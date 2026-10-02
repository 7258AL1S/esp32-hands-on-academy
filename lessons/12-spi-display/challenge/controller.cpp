#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text.rfind("WRITE:", 0) == 0)
            board_.output.display = board_.input.text.substr(6);
    }
}
