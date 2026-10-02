#pragma once
#include "academy/board.hpp"
#include <cstdint>
#include <string>

namespace academy {
// Host experiment fixture. Fields are observations, not ESP32 driver APIs.
// Each lesson uses only the inputs/outputs needed by its small HAL contract.
struct Inputs {
    std::uint32_t now_ms = 0;
    bool button = false;
    int analog = 2048;
    double temperature = 22.0;
    bool connected = true;
    bool fault = false;
    std::uint32_t pulses = 0;
    std::string text;
};
struct Outputs {
    bool led = false;
    int brightness = 0;
    double reading = 0;
    int events = 0;
    std::string state = "idle";
    std::string outgoing;
    std::string display;
    int queue_depth = 0;
    bool error = false;
};
class Device : public Board {
public:
    Inputs input;
    Outputs output;
    Level read_button() const override { return input.button ? Level::low : Level::high; }
    void write_led(bool on) override { output.led = on; }
};
} // namespace academy
