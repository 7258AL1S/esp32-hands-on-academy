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
        t.run("known address ACKs", [] {
            Device d;
            Controller c(d);
            d.input.temperature = 22;
            tx(d, c, "I2C:0x48:READ");
            require(d.output.outgoing.rfind("ACK:", 0) == 0, "ack");
        });
        return t.result();
    }
    t.run("correct address reads sensor", [] {
        Device d;
        Controller c(d);
        d.input.temperature = 23.5;
        tx(d, c, "I2C:0x48:READ");
        near(d.output.reading, 23.5, .01, "temperature");
        require(!d.output.error, "success");
    });
    t.run("unknown address NACKs", [] {
        Device d;
        Controller c(d);
        tx(d, c, "I2C:0x49:READ");
        require(d.output.error && d.output.outgoing == "NACK\n", "nack");
    });
    t.run("missing device times out", [] {
        Device d;
        Controller c(d);
        d.input.connected = false;
        tx(d, c, "I2C:0x48:READ");
        require(d.output.error && d.output.outgoing == "TIMEOUT\n", "timeout");
    });
    return t.result();
}
