#include "controller.hpp"

namespace academy {

void Controller::tick() {
    const bool pressed = board_.read_button() == Level::low;
    if (pressed) {
        led_on_ = !led_on_;
    }
    board_.write_led(led_on_);
}

} // namespace academy
