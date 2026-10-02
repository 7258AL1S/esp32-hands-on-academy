#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        board_.output.events = static_cast<int>(board_.input.pulses);
    }
}
