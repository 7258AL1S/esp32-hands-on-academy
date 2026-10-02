#include "controller.hpp"
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text == "PING\n")
            board_.output.outgoing += "PONG\n";
    }
}
