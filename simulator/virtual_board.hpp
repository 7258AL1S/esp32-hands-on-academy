#pragma once
#include "academy/board.hpp"

namespace academy {

class VirtualBoard final : public Board {
public:
    Level read_button() const override { return button_; }
    void write_led(bool on) override { led_ = on; }
    void set_button(Level level) { button_ = level; }
    bool led() const { return led_; }

private:
    Level button_ = Level::high;
    bool led_ = false;
};

} // namespace academy
