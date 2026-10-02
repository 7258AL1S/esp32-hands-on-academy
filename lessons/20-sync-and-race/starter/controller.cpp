#include "controller.hpp"
namespace academy {
void Controller::tick() {
    if (device_.input.pulses > observed_pulses_) pending_ += static_cast<int>(device_.input.pulses - observed_pulses_);
    observed_pulses_ = device_.input.pulses;
    device_.output.error = device_.input.fault;
    if (device_.input.fault) device_.output.state = "fault";
    else if (!device_.input.connected) device_.output.state = "offline";
    else if (pending_ > 0) { --pending_; ++device_.output.events; device_.output.state = "processing"; }
    else device_.output.state = "waiting";
    device_.output.queue_depth = pending_;
}
}
