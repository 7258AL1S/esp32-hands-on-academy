#include "controller.hpp"
#include "virtual_board.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace {
constexpr std::uint64_t sample_ms = 10;

void show(std::uint64_t time_ms, const academy::VirtualBoard& board) {
    std::cout << "t=" << time_ms << "ms button="
              << (board.read_button() == academy::Level::low ? "LOW" : "HIGH")
              << " led=" << (board.led() ? "ON" : "OFF") << std::endl;
}

// One command -> one observation. A live notebook session owns this process,
// so all button changes and samples execute the learner's compiled Controller.
int protocol_main() {
    academy::VirtualBoard board;
    auto controller = std::make_unique<academy::Controller>(board);
    std::uint64_t time_ms = 0;
    controller->tick();
    show(time_ms, board);
    std::string command;
    while (std::getline(std::cin, command)) {
        if (command == "quit") return 0;
        if (command == "reset" || command == "boot-held") {
            board = academy::VirtualBoard{};
            if (command == "boot-held") board.set_button(academy::Level::low);
            controller = std::make_unique<academy::Controller>(board);
            time_ms = 0;
        } else {
            if (command == "press") board.set_button(academy::Level::low);
            else if (command == "release") board.set_button(academy::Level::high);
            else if (command != "tick") {
                std::cerr << "Unknown protocol command\n";
                return 2;
            }
            time_ms += sample_ms;
        }
        controller->tick();
        show(time_ms, board);
    }
    return 0;
}
} // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--protocol") return protocol_main();
    academy::VirtualBoard board;
    academy::Controller controller(board);
    std::uint64_t time_ms = 0;
    controller.tick();
    show(time_ms, board);
    const auto tick = [&]() {
        time_ms += sample_ms;
        controller.tick();
        show(time_ms, board);
    };

    if (argc == 1) {
        // Released -> first press/hold -> release -> second press/hold -> release.
        for (int sample = 1; sample <= 14; ++sample) {
            const bool pressed = (sample >= 1 && sample <= 6)
                              || (sample >= 8 && sample <= 13);
            board.set_button(pressed ? academy::Level::low : academy::Level::high);
            tick();
        }
        return 0;
    }
    if (argc != 2 || std::string(argv[1]) != "--interactive") {
        std::cerr << "Usage: simulator [--interactive]\n";
        return 2;
    }

    std::cout << "Commands: press, release, wait <ms>, status, quit\n"
                 "Virtual time only; press/release sample after 10 ms.\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command, extra;
        input >> command;
        if (command.empty()) {
            continue;
        }
        if (command == "wait") {
            int duration = -1;
            if (!(input >> duration) || (input >> extra) || duration < 0
                || duration > 60000 || duration % 10 != 0) {
                std::cerr << "wait requires 0..60000 ms, a multiple of 10.\n";
                continue;
            }
            for (int elapsed = 0; elapsed < duration; elapsed += 10) {
                tick();
            }
        } else if (input >> extra) {
            std::cerr << "Unexpected argument.\n";
        } else if (command == "press" || command == "release") {
            board.set_button(command == "press" ? academy::Level::low
                                                : academy::Level::high);
            tick();
        } else if (command == "status") {
            show(time_ms, board);
        } else if (command == "quit") {
            break;
        } else {
            std::cerr << "Unknown command. Try press, release, wait, status, quit.\n";
        }
    }
    return 0;
}
