#include "controller.hpp"
namespace academy {
void Controller::tick() {
    const int interval = device_.input.analog * 1000;
    device_.output.error = interval < 1000 || interval > 3600000 || device_.input.text.empty();
    device_.output.state = device_.output.error ? "invalid-config" : "ready-to-save";
    device_.output.display = device_.input.text + " / " + std::to_string(interval) + " ms";
}
}
