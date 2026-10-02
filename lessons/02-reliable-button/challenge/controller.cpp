#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& board) : board_(board) {
    }
    void Controller::tick() {
        const bool pressed = board_.read_button() == Level::low;
        if (!initialized_) {
            initialized_ = true;
            raw_ = pressed;
        }
        if (pressed && !raw_) {
            ++board_.output.events;
            board_.output.led = !board_.output.led;
        }
        raw_ = pressed;
        board_.output.state = "raw edge";
    }
}
