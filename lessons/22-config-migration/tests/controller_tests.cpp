#include "academy/test.hpp"
#include "academy/system/config_store.hpp"
#include "controller.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
using academy::Controller; using academy::Device;
namespace fs = std::filesystem;
static fs::path sandbox(const char* name) { auto p=fs::temp_directory_path()/"academy-config-tests"/name; fs::remove_all(p); fs::create_directories(p); return p; }
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("拒绝危险的配置间隔", [] { Device d; d.input.text="desk"; d.input.analog=0; Controller c(d); c.tick(); academy::require(d.output.error, "零间隔必须在写盘前失败"); });
    suite.run("保存后可真实读回", [] { auto p=sandbox("roundtrip"); academy::system::ConfigStore store(p); store.save({2,"desk",5000}); const auto loaded=store.load(); academy::require(loaded.config.device_name=="desk" && loaded.config.report_interval_ms==5000, "写盘或读回错误"); });
    suite.run("v1 配置迁移为 v2", [] { auto p=sandbox("migration"); std::ofstream out(p/"device.conf"); out<<"version=1\nname=old\ninterval_s=7\n"; out.close(); academy::system::ConfigStore store(p); const auto loaded=store.load(); academy::require(loaded.migrated && loaded.config.report_interval_ms==7000, "没有执行版本迁移"); academy::require(store.load().config.version==2, "迁移结果没有落盘"); });
    return suite.result();
}
