#include "controller.hpp"
#include "virtual_board.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using academy::Level;

namespace {
struct Rig {
    academy::VirtualBoard board;
    academy::Controller controller{board};

    void sample(Level level, bool expected, const std::string& context) {
        board.set_button(level);
        controller.tick();
        if (board.led() != expected) {
            throw std::runtime_error(context + ": expected LED "
                + (expected ? "ON" : "OFF") + ", observed "
                + (board.led() ? "ON" : "OFF"));
        }
    }
};

struct Case {
    const char* name;
    std::function<void()> run;
};

std::vector<Case> guided_cases() {
    return {
        {"Released input -> LED OFF", [] {
            Rig rig;
            rig.sample(Level::high, false, "initial released sample");
        }},
        {"LOW input -> LED ON", [] {
            Rig rig;
            rig.sample(Level::high, false, "released");
            rig.sample(Level::low, true, "press");
        }},
        {"Held input remains ON", [] {
            Rig rig;
            for (int i = 0; i < 20; ++i) {
                rig.sample(Level::low, true, "hold sample " + std::to_string(i));
            }
        }},
        {"Release -> LED OFF", [] {
            Rig rig;
            rig.sample(Level::low, true, "press");
            rig.sample(Level::high, false, "release");
        }},
    };
}

std::vector<Case> exercise_cases() {
    return {
        {"Power-on released -> LED OFF", [] {
            Rig rig;
            for (int i = 0; i < 5; ++i) rig.sample(Level::high, false, "idle");
        }},
        {"First press -> LED ON", [] {
            Rig rig;
            rig.sample(Level::high, false, "baseline");
            rig.sample(Level::low, true, "first press");
        }},
        {"Long hold causes only one toggle", [] {
            Rig rig;
            rig.sample(Level::high, false, "baseline");
            for (int i = 0; i < 100; ++i) {
                rig.sample(Level::low, true, "hold sample " + std::to_string(i));
            }
        }},
        {"Release preserves LED state", [] {
            Rig rig;
            rig.sample(Level::high, false, "baseline");
            rig.sample(Level::low, true, "press");
            for (int i = 0; i < 5; ++i) rig.sample(Level::high, true, "release");
        }},
        {"Second press -> LED OFF, next press -> ON", [] {
            Rig rig;
            rig.sample(Level::high, false, "baseline");
            rig.sample(Level::low, true, "first press");
            rig.sample(Level::high, true, "first release");
            rig.sample(Level::low, false, "second press");
            rig.sample(Level::low, false, "second hold");
            rig.sample(Level::high, false, "second release");
            rig.sample(Level::low, true, "third press");
        }},
        {"Held at power-on is a baseline, not a new press", [] {
            Rig rig;
            rig.sample(Level::low, false, "power-on held");
            rig.sample(Level::low, false, "still held");
            rig.sample(Level::high, false, "release after power-on");
            rig.sample(Level::low, true, "first new press");
        }},
        {"Two controllers have independent state", [] {
            Rig first, second;
            first.sample(Level::high, false, "first baseline");
            first.sample(Level::low, true, "first press");
            second.sample(Level::high, false, "second baseline");
            second.sample(Level::low, true, "second press");
            first.sample(Level::high, true, "first release");
            first.sample(Level::low, false, "first second press");
            second.sample(Level::low, true, "second remains held");
        }},
        {"100 complete presses toggle by press count", [] {
            Rig rig;
            rig.sample(Level::high, false, "baseline");
            for (int count = 1; count <= 100; ++count) {
                const bool expected = count % 2 == 1;
                rig.sample(Level::low, expected, "press " + std::to_string(count));
                for (int hold = 0; hold < count % 7; ++hold) {
                    rig.sample(Level::low, expected, "varying hold");
                }
                rig.sample(Level::high, expected, "release");
            }
        }},
    };
}
} // namespace

int main(int argc, char** argv) {
    if (argc != 2 || (std::string(argv[1]) != "guided"
                     && std::string(argv[1]) != "exercise")) {
        std::cerr << "Usage: controller_tests guided|exercise\n";
        return 2;
    }
    const auto cases = std::string(argv[1]) == "guided" ? guided_cases() : exercise_cases();
    int failed = 0;
    for (const auto& test : cases) {
        try {
            test.run();
            std::cout << "[PASS] " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cout << "[FAIL] " << test.name << "\n       " << error.what() << '\n';
        }
    }
    std::cout << cases.size() - failed << '/' << cases.size() << " checks passed\n";
    return failed == 0 ? 0 : 1;
}
