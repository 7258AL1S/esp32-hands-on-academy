#include "academy/device.hpp"
#include "academy/test.hpp"
#include "controller.hpp"
#include <string>
using namespace academy;
static void tx(Device& d, Controller& c, const char* s) {
    d.input.text = s;
    c.tick();
}
int main(int argc, char** argv) {
    std::string p = argc == 2 ? argv[1] : "exercise";
    TestSuite t;
    if (p == "guided") {
        t.run("WRITE changes basic display", [] {
            Device d;
            Controller c(d);
            tx(d, c, "WRITE:Hi");
            require(d.output.display == "Hi", "display");
        });
        return t.result();
    }
    t.run("CS frames commit only on high", [] {
        Device d;
        Controller c(d);
        tx(d, c, "CS:LOW");
        tx(d, c, "WRITE:Hel");
        tx(d, c, "WRITE:lo");
        require(d.output.display.empty(), "not committed");
        tx(d, c, "CS:HIGH");
        require(d.output.display == "Hello", "frame");
    });
    t.run("unselected write is error", [] {
        Device d;
        Controller c(d);
        tx(d, c, "WRITE:bad");
        require(d.output.error && d.output.display.empty(), "cs required");
    });
    t.run("fault cancels transaction", [] {
        Device d;
        Controller c(d);
        tx(d, c, "CS:LOW");
        d.input.fault = true;
        tx(d, c, "WRITE:x");
        require(d.output.error && d.output.display.empty(), "fault");
    });
    return t.result();
}
