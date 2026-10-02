#include "academy/net/socket.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_tcp_line_server([&](const std::string& request) {
        if (request == "STATUS\n") {
            return std::string("reminder=") + (reminder_on ? "ON\n" : "OFF\n");
        }
        if (request == "TOGGLE\n") {
            reminder_on = !reminder_on;
            return std::string("reminder=") + (reminder_on ? "ON\n" : "OFF\n");
        }
        return std::string("ERR unknown command\n");
    });
}
