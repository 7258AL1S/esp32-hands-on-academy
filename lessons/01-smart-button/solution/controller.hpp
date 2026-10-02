#pragma once
#include "academy/board.hpp"

namespace academy {

class Controller {
public:
    explicit Controller(Board& board) : board_(board) {}
    void tick();

private:
    Board& board_;
    bool led_on_ = false;
    bool was_pressed_ = false;
    bool initialized_ = false;
};

} // namespace academy
