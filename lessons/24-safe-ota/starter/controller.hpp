#pragma once
#include "academy/device.hpp"
namespace academy {
    class Controller {
    public:
        explicit Controller(Device& device) : device_(device) {
        }
        void tick();

    private:
        Device& device_;
    };
}
