#pragma once
#include "academy/device.hpp"
namespace academy { class Controller { public: explicit Controller(Device& d):device_(d){} void tick(){ device_.output.state="idle"; device_.output.led=false; } private: Device& device_; }; }
