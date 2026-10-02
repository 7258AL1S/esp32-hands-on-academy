#include "controller.hpp"
#include "academy/test.hpp"
using namespace academy;
int main(int argc, char** argv) {
    TestSuite t;
    std::string p = argc == 2 ? argv[1] : "exercise";
    t.run("initial project contract", [] {
        Device d;
        Controller c(d);
        c.tick();
        require(d.output.state == "idle" && !d.output.led, "initial state");
    });
    return t.result();
}
