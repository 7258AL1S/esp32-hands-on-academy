#include "controller.hpp"
namespace academy {
void Controller::tick() {
    if (device_.input.button && !previous_button_) ++pending_notifications_;
    previous_button_ = device_.input.button;
    device_.output.error = device_.input.fault;
    if (device_.input.fault) device_.output.state = "fault";
    else if (!device_.input.connected) device_.output.state = "waiting-link";
    else if (pending_notifications_ > 0) {
        --pending_notifications_;
        ++device_.output.events;
        device_.output.outgoing = "notification";
        device_.output.state = "sent";
    } else device_.output.state = "idle";
    device_.output.queue_depth = pending_notifications_;
}
}
