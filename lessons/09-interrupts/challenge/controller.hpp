#pragma once
#include "academy/device.hpp"
#include <cstdint>
namespace academy {class Controller{public:explicit Controller(Device&);void tick();private:Device&board_;bool init_=false;std::uint32_t seen_=0;};}
