#pragma once
#include "academy/device.hpp"
namespace academy {
class Controller {
public:
    explicit Controller(Device& device) : device_(device) {}
    void tick();
private:
    Device& device_;
    std::uint32_t next_sensor_ms_ = 0;
    std::uint32_t next_network_ms_ = 0;
    bool previous_button_ = false;
    int pending_samples_ = 0;
};
}
