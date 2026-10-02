#include "controller.hpp"
namespace academy {
    void Controller::tick() {
        // `text` is a model of a validated GATT characteristic write, not radio traffic.
        if (!device_.input.connected) {
            device_.output.state = "advertising";
            return;
        }
        if (device_.input.text.rfind("wifi:", 0) == 0 &&
            device_.input.text.size() > 5) {
            provisioned_ = true;
            device_.output.outgoing = "provisioning-status:accepted";
            ++device_.output.events;
        } else if (!device_.input.text.empty()) {
            device_.output.error = true;
            device_.output.outgoing = "provisioning-status:rejected";
        }
        device_.output.state = provisioned_ ? "provisioned" : "connected";
        device_.output.display =
            "Service: Provisioning / Characteristic: WiFiCredentials";
    }
}
