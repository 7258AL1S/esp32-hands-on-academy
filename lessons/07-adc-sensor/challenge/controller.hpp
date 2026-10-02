#pragma once
#include "academy/device.hpp"
namespace academy {
    class Controller {
    public:
        explicit Controller(Device&);
        void tick();

    private:
        Device& board_;
    };
}
