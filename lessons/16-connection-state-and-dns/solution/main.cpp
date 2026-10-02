#include "academy/net/socket.hpp"
#include <iostream>

int main() {
    academy::net::SocketSystem sockets;
    if (!academy::net::env_flag("ACADEMY_WIFI_CONNECTED")) {
        std::cout << "WAIT_FOR_WIFI\n";
        return 0;
    }
    const auto address = academy::net::resolve_ipv4("localhost");
    const auto reply = academy::net::request_line(
        "localhost", academy::net::env_port("ACADEMY_PORT"), "PING\n");
    std::cout << "RESOLVED " << address << ' ' << reply;
    return reply == "PONG\n" ? 0 : 1;
}
