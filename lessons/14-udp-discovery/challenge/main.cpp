#include "academy/net/socket.hpp"

int main() {
    return academy::net::run_udp_server([](std::string_view packet) {
        if (packet == "DISCOVER")
            return std::string("DESK_REMINDER|ONLINE\n");
        return std::string("ERR");
    });
}
