#pragma once
#include "academy/device.hpp"
#include <string>
namespace academy {class Controller{public:explicit Controller(Device&);void tick();private:Device&board_;bool selected_=false;std::string frame_;};}
