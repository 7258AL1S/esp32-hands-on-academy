#include "controller.hpp"
#include <string>
namespace academy {
    void Controller::tick() {
        const auto now = device_.input.now_ms;
        if (now >= next_sensor_ms_) {
            device_.output.reading = device_.input.temperature;
            ++pending_samples_;
            next_sensor_ms_ = now + 100;
        }
        if (device_.input.connected && pending_samples_ > 0 &&
            now >= next_network_ms_) {
            --pending_samples_;
            ++device_.output.events;
            device_.output.outgoing = "temperature sample";
            next_network_ms_ = now + 50;
        }
        if (device_.input.button && !previous_button_)
            device_.output.led = !device_.output.led;
        previous_button_ = device_.input.button;
        device_.output.queue_depth = pending_samples_;
        device_.output.state = device_.input.connected ? "online" : "offline";
    }
}
