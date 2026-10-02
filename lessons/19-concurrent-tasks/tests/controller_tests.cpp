#include "academy/test.hpp"
#include "controller.hpp"
using academy::Controller;
using academy::Device;
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("离线时传感器仍按周期采样", [] {
        Device d;
        d.input.connected = false;
        Controller c(d);
        c.tick();
        d.input.now_ms = 100;
        d.input.temperature = 24;
        c.tick();
        academy::require(d.output.queue_depth == 2 && d.output.reading == 24,
                         "离线采样或缓存丢失");
    });
    suite.run("恢复连接后逐项发送缓存", [] {
        Device d;
        d.input.connected = false;
        Controller c(d);
        c.tick();
        d.input.now_ms = 100;
        c.tick();
        d.input.connected = true;
        d.input.now_ms = 150;
        c.tick();
        academy::require(d.output.events == 1 && d.output.queue_depth == 1,
                         "网络一次只能取走一个样本");
    });
    suite.run("按钮不等待网络任务", [] {
        Device d;
        d.input.connected = false;
        Controller c(d);
        d.input.button = true;
        c.tick();
        academy::require(d.output.led, "按钮响应被后台任务阻塞");
    });
    return suite.result();
}
