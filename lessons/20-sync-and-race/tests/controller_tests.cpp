#include "academy/test.hpp"
#include "controller.hpp"
using academy::Controller;
using academy::Device;
int main(int, char**) {
    academy::TestSuite suite;
    suite.run("队列保留离线期间的全部事件", [] {
        Device d;
        d.input.connected = false;
        Controller c(d);
        d.input.pulses = 2;
        c.tick();
        d.input.pulses = 5;
        c.tick();
        academy::require(d.output.queue_depth == 5, "事件被覆盖而不是追加");
    });
    suite.run("消费者每次只处理一条消息", [] {
        Device d;
        d.input.pulses = 3;
        Controller c(d);
        c.tick();
        academy::require(d.output.events == 1 && d.output.queue_depth == 2,
                         "消息边界不正确");
    });
    suite.run("故障不偷偷消费队列", [] {
        Device d;
        d.input.pulses = 2;
        d.input.fault = true;
        Controller c(d);
        c.tick();
        academy::require(d.output.error && d.output.queue_depth == 2,
                         "故障路径破坏队列");
    });
    return suite.result();
}
