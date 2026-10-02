#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        const std::string cmd = board_.input.text;
        if (cmd != last_command_) {
            last_command_ = cmd;
            if (cmd == "start" && state_ == "idle") {
                state_ = "running";
                end_ = board_.input.now_ms + 1000;
                ++board_.output.events;
            } else if (cmd == "pause" && state_ == "running") {
                remaining_ = end_ - board_.input.now_ms;
                state_ = "paused";
                ++board_.output.events;
            } else if (cmd == "resume" && state_ == "paused") {
                end_ = board_.input.now_ms + remaining_;
                state_ = "running";
                ++board_.output.events;
            } else if (cmd == "reset") {
                state_ = "idle";
                remaining_ = 0;
                ++board_.output.events;
            }
        }
        if (state_ == "running" && board_.input.now_ms >= end_) {
            state_ = "done";
        }
        board_.output.state = state_;
        board_.output.led = state_ == "running";
        board_.output.reading = state_ == "running"
                                    ? end_ - board_.input.now_ms
                                    : (state_ == "paused" ? remaining_ : 0);
    }
}
