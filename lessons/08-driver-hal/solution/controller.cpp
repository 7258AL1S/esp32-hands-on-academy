#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (!board_.input.connected || board_.input.fault) {
            board_.output.error = true;
            board_.output.led = false;
            board_.output.state = "sensor error";
            return;
        }
        board_.output.reading = board_.input.temperature + .5;
        board_.output.error = false;
        board_.output.led = board_.output.reading >= 28.;
        board_.output.state = "ok";
    }
}
