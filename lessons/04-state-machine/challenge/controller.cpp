#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text == last_command_) {
            return;
        }
        last_command_ = board_.input.text;
        if (board_.input.text == "start") {
            state_ = "running";
            end_ = board_.input.now_ms + 1000;
        }
        if (board_.input.text == "pause") {
            state_ = "paused";
        }
        if (board_.input.text == "reset") {
            state_ = "idle";
        }
        if (board_.input.now_ms >= end_) {
            state_ = "done";
        }
        board_.output.state = state_;
        board_.output.led = state_ == "running";
    }
}
