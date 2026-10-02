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
        if (board_.input.pulses < seen_) {
            board_.output.error = true;
            return;
        }
        std::uint32_t delta = board_.input.pulses - seen_;
        seen_ = board_.input.pulses;
        board_.output.events += static_cast<int>(delta);
        if (delta)
            board_.output.led = (board_.output.events % 2) != 0;
        board_.output.state = "events";
    }
}
