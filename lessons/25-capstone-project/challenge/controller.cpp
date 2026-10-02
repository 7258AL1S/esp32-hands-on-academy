#include "controller.hpp"
namespace academy {
    void Controller::tick() {
        device_.output.state = "broken";
        device_.output.led = true;
    }
}
