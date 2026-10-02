#include "academy/net/socket.hpp"

int main() {
    return academy::net::run_tcp_line_server([](const std::string& request) {
        bool reminder_on = false; // 每条连接都会重新创建它。
        if (request == "TOGGLE\n")
            reminder_on = !reminder_on;
        return std::string("reminder=") + (reminder_on ? "ON\n" : "OFF\n");
    });
}
