#include "controller.hpp"
namespace academy {
void Controller::tick() {
    // Bug: a filename is mistakenly treated as proof that the package is safe.
    if (device_.input.text.empty()) { device_.output.error=true; device_.output.state="no-package"; return; }
    device_.output.state = "activated-without-check";
    device_.output.display = "unsafe: no integrity check";
}
}
