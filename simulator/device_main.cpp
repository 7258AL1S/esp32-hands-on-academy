#include "controller.hpp"
#include "academy/device.hpp"
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cmath>

namespace {
std::string unhex(const std::string& value) {
    if (value == "-") return {};
    if (value.size() % 2) throw std::runtime_error("Invalid text input");
    std::string result;
    for (std::size_t i = 0; i < value.size(); i += 2) {
        const auto digit = [](char c) -> unsigned {
            if (c >= '0' && c <= '9') return static_cast<unsigned>(c - '0');
            if (c >= 'a' && c <= 'f') return static_cast<unsigned>(c - 'a' + 10);
            throw std::runtime_error("Invalid text input");
        };
        result += static_cast<char>((digit(value[i]) << 4) | digit(value[i + 1]));
    }
    return result;
}
std::string json_string(const std::string& value) {
    std::ostringstream out;
    out << '"';
    for (unsigned char c : value) {
        if (c == '"' || c == '\\') out << '\\' << c;
        else if (c < 32) out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(c) << std::dec;
        else out << c;
    }
    out << '"';
    return out.str();
}
}

int main() {
    academy::Device device;
    auto controller = std::make_unique<academy::Controller>(device);
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit") return 0;
        if (line == "reset") {
            device = academy::Device{};
            controller = std::make_unique<academy::Controller>(device);
        } else {
            std::istringstream in(line);
            academy::Inputs input;
            if (!(in >> input.now_ms >> input.button >> input.analog >> input.temperature
                     >> input.connected >> input.fault >> input.pulses >> input.text)) return 2;
            input.text = unhex(input.text);
            device.input = input;
        }
        controller->tick();
        const auto& o = device.output;
        if (!std::isfinite(o.reading)) return 3;
        std::cout << "{\"now_ms\":" << device.input.now_ms << ",\"led\":" << o.led
                  << ",\"brightness\":" << o.brightness << ",\"reading\":" << std::setprecision(12) << o.reading
                  << ",\"events\":" << o.events << ",\"state\":" << json_string(o.state)
                  << ",\"outgoing\":" << json_string(o.outgoing) << ",\"display\":" << json_string(o.display)
                  << ",\"queue_depth\":" << o.queue_depth << ",\"error\":" << o.error << "}" << std::endl;
    }
}
