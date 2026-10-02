#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        board_.output.brightness = board_.input.analog;
        board_.output.led = true;
    }
}
