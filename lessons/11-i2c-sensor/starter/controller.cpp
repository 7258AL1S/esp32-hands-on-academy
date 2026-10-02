#include "controller.hpp"
#include <string>
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text == "I2C:0x48:READ") {
            board_.output.reading = board_.input.temperature;
            board_.output.outgoing =
                "ACK:" + std::to_string(board_.output.reading) + "\n";
        }
    }
}
