#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        board_.output.reading = board_.input.analog * 3.3 / 4095.0;
        board_.output.led = board_.output.reading >= 2.;
    }
}
