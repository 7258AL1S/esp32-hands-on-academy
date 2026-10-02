#include "academy/test.hpp"
#include "academy/system/ota_sandbox.hpp"
#include "controller.hpp"
#include <filesystem>
#include <fstream>
using academy::Controller; using academy::Device; namespace fs=std::filesystem;
static fs::path box(const char* name) { auto p=fs::temp_directory_path()/"academy-ota-tests"/name; fs::remove_all(p); fs::create_directories(p); return p; }
static void package(const fs::path& p, const std::string& body, bool valid) { fs::create_directories(p); std::ofstream(p/"firmware.bin",std::ios::binary)<<body; const auto hash=academy::system::Sha256::file_hex(p/"firmware.bin"); std::ofstream m(p/"manifest.txt"); m<<"version=2.0\nsha256="<<(valid?hash:std::string(64,'0'))<<"\n"; }
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("控制器要求先校验", [] { Device d; d.input.text="candidate"; Controller c(d); c.tick(); academy::require(d.output.state=="verify-before-activate", "不能跳过校验"); });
    suite.run("篡改包不会进入候选槽", [] { auto root=box("tampered"); auto p=root/"pkg"; package(p,"firmware",false); academy::system::OtaSandbox ota(root/"device"); const auto result=ota.stage(p); academy::require(!result.ok && !fs::exists(root/"device/staging/firmware.bin"), "错误包被写入"); });
    suite.run("失败启动保留原槽并回滚", [] { auto root=box("rollback"); auto p=root/"pkg"; package(p,"firmware",true); academy::system::OtaSandbox ota(root/"device"); academy::require(ota.stage(p).ok && ota.activate().ok, "无法准备候选包"); const auto result=ota.boot(false); academy::require(result.state=="rolled_back" && ota.current_slot()=="A", "回滚没有保留已知可用槽"); });
    suite.run("健康检查确认新槽", [] { auto root=box("confirm"); auto p=root/"pkg"; package(p,"firmware",true); academy::system::OtaSandbox ota(root/"device"); ota.stage(p); ota.activate(); academy::require(ota.boot(true).state=="confirmed" && ota.current_slot()=="B", "健康检查后未确认"); });
    return suite.result();
}
