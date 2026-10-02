#pragma once
#include "academy/device.hpp"
#include <array>
#include <string>
namespace academy {
    class Controller {
    public:
        explicit Controller(Device&);
        void tick();

    private:
        Device& board_;
        std::array<int, 4> data_{};
        int count_ = 0;
        std::string last_;
    };
}
