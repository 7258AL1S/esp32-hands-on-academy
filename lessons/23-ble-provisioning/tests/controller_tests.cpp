#include "academy/test.hpp"
#include "controller.hpp"
using academy::Controller; using academy::Device;
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("未连接时只公布服务", [] { Device d; d.input.connected=false; Controller c(d); c.tick(); academy::require(d.output.state=="advertising" && d.output.events==0, "离线状态错误"); });
    suite.run("有效特征写入触发确认", [] { Device d; d.input.text="wifi:MakerNet"; Controller c(d); c.tick(); academy::require(d.output.state=="provisioned" && d.output.outgoing=="provisioning-status:accepted", "写入没有被确认"); });
    suite.run("空凭据不会配网成功", [] { Device d; d.input.text="wifi:"; Controller c(d); c.tick(); academy::require(d.output.error && d.output.state=="connected", "空凭据被接受"); });
    return suite.result();
}
