#include "controller.hpp"

namespace academy {

void Controller::tick() {
    const bool pressed = board_.read_button() == Level::low;
    board_.write_led(pressed);
}

} // namespace academy
