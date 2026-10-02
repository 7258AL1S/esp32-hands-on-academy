#pragma once
#include "academy/board.hpp"

namespace academy {

class Controller {
public:
    explicit Controller(Board& board) : board_(board) {}
    void tick();

private:
    Board& board_;
    // Exercise: add persistent state here when you need it.
};

} // namespace academy
