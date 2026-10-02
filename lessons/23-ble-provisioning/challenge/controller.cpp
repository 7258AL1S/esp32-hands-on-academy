#include "controller.hpp"
namespace academy {
void Controller::tick() {
    if (!device_.input.connected) { device_.output.state="advertising"; return; }
    // Bug: accepts an empty credential write as provisioning success.
    if (device_.input.text.rfind("wifi:", 0) == 0) { provisioned_=true; device_.output.outgoing="provisioning-status:accepted"; ++device_.output.events; }
    else if (!device_.input.text.empty()) { device_.output.error=true; device_.output.outgoing="provisioning-status:rejected"; }
    device_.output.state=provisioned_?"provisioned":"connected";
    device_.output.display="Service: Provisioning / Characteristic: WiFiCredentials";
}
}
