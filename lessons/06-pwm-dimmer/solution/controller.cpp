#include "controller.hpp"
#include <algorithm>
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.fault) {
            board_.output.brightness = 0;
            board_.output.led = false;
            board_.output.error = true;
            board_.output.state = "fault";
            return;
        }
        int raw = std::clamp(board_.input.analog, 0, 4095);
        board_.output.brightness = (raw * 100 + 2047) / 4095;
        board_.output.led = board_.output.brightness > 0;
        board_.output.error = false;
        board_.output.state = "pwm";
    }
}
