#include "controller.hpp"
#include <cstdlib>
namespace academy {
    Controller::Controller(Device& b) : board_(b) {
    }
    void Controller::tick() {
        if (board_.input.text == last_)
            return;
        last_ = board_.input.text;
        if (last_.empty())
            return;
        board_.output.error = false;
        if (last_ == "clear") {
            count_ = 0;
            board_.output.reading = 0;
        } else if (last_ == "average") {
            if (!count_) {
                board_.output.error = true;
            } else {
                int sum = 0;
                for (int i = 0; i < count_; ++i)
                    sum += data_[i];
                board_.output.reading = static_cast<double>(sum) / count_;
            }
        } else if (last_.rfind("push:", 0) == 0) {
            if (count_ == static_cast<int>(data_.size()))
                board_.output.error = true;
            else
                data_[count_++] = std::atoi(last_.c_str() + 5);
        } else
            board_.output.error = true;
        board_.output.queue_depth = count_;
        board_.output.state =
            board_.output.error ? "error" : (count_ == 4 ? "full" : "ready");
    }
}
