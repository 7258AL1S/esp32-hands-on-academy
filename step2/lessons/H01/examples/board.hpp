#pragma once

namespace academy {

    enum class Level {
        low,
        high
    };

    // Same contract as STEP 1. LOW means pressed; configure before first read.
    class Board {
    public:
        virtual ~Board() = default;
        virtual Level read_button() const = 0;
        virtual void write_led(bool on) = 0;
    };

} // namespace academy
