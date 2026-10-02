#pragma once
#include "academy/device.hpp"
namespace academy {
class Controller {
public:
    explicit Controller(Device& device) : device_(device) {}
    void tick();
private:
    Device& device_;
    std::uint32_t observed_pulses_ = 0;
    int pending_ = 0;
};
}
