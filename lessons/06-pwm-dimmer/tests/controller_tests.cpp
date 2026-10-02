#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;
int main(int argc, char** argv) {
    std::string p = argc == 2 ? argv[1] : "exercise";
    TestSuite t;
    if (p == "guided") {
        t.run("middle input is about half", [] {
            Device d;
            Controller c(d);
            d.input.analog = 2048;
            c.tick();
            require(d.output.brightness >= 49 && d.output.brightness <= 50,
                    "half duty");
        });
        return t.result();
    }
    t.run("mapping is bounded", [] {
        Device d;
        Controller c(d);
        d.input.analog = -4;
        c.tick();
        require(d.output.brightness == 0 && !d.output.led, "low clamp");
        d.input.analog = 9000;
        c.tick();
        require(d.output.brightness == 100 && d.output.led, "high clamp");
    });
    t.run("fault disables output", [] {
        Device d;
        Controller c(d);
        d.input.analog = 4095;
        d.input.fault = true;
        c.tick();
        require(d.output.error && d.output.brightness == 0 && !d.output.led,
                "safe fault");
    });
    return t.result();
}
