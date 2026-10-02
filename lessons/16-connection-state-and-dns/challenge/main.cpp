#include "academy/net/socket.hpp"
#include <iostream>

int main() {
    academy::net::SocketSystem sockets;
    // 错误：无论模型是否已连接，都尝试连接服务。
    const auto address = academy::net::resolve_ipv4("localhost");
    const auto reply = academy::net::request_line("localhost", academy::net::env_port("ACADEMY_PORT"), "PING\n");
    std::cout << "RESOLVED " << address << ' ' << reply;
    return 0;
}
