#include "controller.hpp"
#include <algorithm>
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.fault) {
            board_.output.error = true;
            board_.output.led = false;
            board_.output.state = "fault";
            return;
        }
        int raw = std::clamp(board_.input.analog, 0, 4095);
        board_.output.reading = raw * 3.3 / 4095.0;
        board_.output.led = board_.output.reading >= 2.0;
        board_.output.error = false;
        board_.output.state = "ok";
    }
}
