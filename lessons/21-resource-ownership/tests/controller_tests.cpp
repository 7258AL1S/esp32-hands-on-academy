#include "academy/test.hpp"
#include "controller.hpp"
using academy::Controller;
using academy::Device;
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("一次按下只有一个通知", [] {
        Device d;
        Controller c(d);
        d.input.button = true;
        c.tick();
        c.tick();
        c.tick();
        academy::require(d.output.events == 1, "保持按住重复提交");
    });
    suite.run("离线时由资源所有者保留请求", [] {
        Device d;
        d.input.connected = false;
        Controller c(d);
        d.input.button = true;
        c.tick();
        d.input.button = false;
        c.tick();
        d.input.connected = true;
        c.tick();
        academy::require(d.output.events == 1 && d.output.queue_depth == 0,
                         "离线请求没有恢复");
    });
    suite.run("故障时不写共享输出", [] {
        Device d;
        d.input.fault = true;
        Controller c(d);
        d.input.button = true;
        c.tick();
        academy::require(d.output.events == 0 && d.output.error, "故障时仍发送");
    });
    return suite.result();
}
