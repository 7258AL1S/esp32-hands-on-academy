#include "controller.hpp"

namespace academy {

void Controller::tick() {
    const bool pressed = board_.read_button() == Level::low;
    if (!initialized_) {
        // Treat the first sample as a baseline, even if held at power-on.
        was_pressed_ = pressed;
        initialized_ = true;
    } else if (pressed && !was_pressed_) {
        led_on_ = !led_on_;
    }
    was_pressed_ = pressed;
    board_.write_led(led_on_);
}

} // namespace academy
