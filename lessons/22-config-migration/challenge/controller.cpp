#include "controller.hpp"
namespace academy {
void Controller::tick() {
    const int interval = device_.input.analog * 1000;
    // Bug: accepts zero/invalid intervals, which makes a later scheduler loop spin.
    device_.output.error = device_.input.text.empty();
    device_.output.state = device_.output.error ? "invalid-config" : "ready-to-save";
    device_.output.display = device_.input.text + " / " + std::to_string(interval) + " ms";
}
}
