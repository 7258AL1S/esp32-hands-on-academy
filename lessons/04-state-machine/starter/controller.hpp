#pragma once
#include "academy/device.hpp"
#include <cstdint>
#include <string>
namespace academy { class Controller { public: explicit Controller(Device& board); void tick(); private: Device& board_; std::string last_command_; std::string state_="idle"; std::uint32_t end_=0, remaining_=0; }; }
