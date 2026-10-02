#include "controller.hpp"
namespace academy {
void Controller::tick() {
    if (device_.input.fault) { device_.output.error=true; device_.output.state="rolled-back"; return; }
    if (device_.input.text.empty()) { device_.output.error=true; device_.output.state="no-package"; return; }
    device_.output.state = "verify-before-activate";
    device_.output.display = "SHA-256 -> inactive slot -> health check -> confirm";
}
}
