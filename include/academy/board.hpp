#pragma once

namespace academy {

enum class Level { low, high };

// Electrical polarity belongs to the board contract: LOW means pressed.
// A Board must configure its input before the first read.
class Board {
public:
    virtual ~Board() = default;
    virtual Level read_button() const = 0;
    virtual void write_led(bool on) = 0;
};

} // namespace academy
