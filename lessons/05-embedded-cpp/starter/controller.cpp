#include "controller.hpp"
#include <cstdlib>
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text == last_)
            return;
        last_ = board_.input.text;
        if (last_.rfind("push:", 0) == 0) {
            data_[count_++] = std::atoi(last_.c_str() + 5);
            board_.output.queue_depth = count_;
        }
    }
}
