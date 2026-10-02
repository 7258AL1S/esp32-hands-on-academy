#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;
static void cmd(Device& d, Controller& c, const char* s) {
    d.input.text = s;
    c.tick();
    d.input.text = "";
    c.tick();
}
int main(int argc, char** argv) {
    std::string p = argc == 2 ? argv[1] : "exercise";
    TestSuite t;
    if (p == "guided") {
        t.run("one sample is stored", [] {
            Device d;
            Controller c(d);
            cmd(d, c, "push:25");
            require(d.output.queue_depth == 1, "one sample");
        });
        return t.result();
    }
    t.run("fixed buffer reports full and rejects fifth", [] {
        Device d;
        Controller c(d);
        for (int i = 1; i <= 4; ++i)
            cmd(d, c, ("push:" + std::to_string(i)).c_str());
        cmd(d, c, "push:5");
        require(d.output.queue_depth == 4 && d.output.error, "fifth must fail");
    });
    t.run("average uses stored samples", [] {
        Device d;
        Controller c(d);
        cmd(d, c, "push:10");
        cmd(d, c, "push:20");
        cmd(d, c, "average");
        near(d.output.reading, 15, .01, "average");
    });
    t.run("invalid command is visible", [] {
        Device d;
        Controller c(d);
        cmd(d, c, "oops");
        require(d.output.error, "invalid command");
    });
    return t.result();
}
