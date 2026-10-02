#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (!init_) {
            init_ = true;
            seen_ = board_.input.pulses;
            return;
        }
        if (board_.input.pulses != seen_) {
            ++board_.output.events;
            seen_ = board_.input.pulses;
        }
    }
}
