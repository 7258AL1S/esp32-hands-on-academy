#pragma once
#include "academy/device.hpp"
#include <cstdint>
namespace academy { class Controller { public: explicit Controller(Device& board); void tick(); private: Device& board_; bool initialized_ = false; bool raw_ = false; bool stable_ = false; std::uint32_t changed_at_ = 0; }; }
