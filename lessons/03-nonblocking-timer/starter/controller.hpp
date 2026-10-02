#pragma once
#include "academy/device.hpp"
#include <cstdint>
namespace academy { class Controller { public: explicit Controller(Device& board); void tick(); private: Device& board_; bool initialized_=false, last_=false, running_=false; std::uint32_t deadline_=0; }; }
