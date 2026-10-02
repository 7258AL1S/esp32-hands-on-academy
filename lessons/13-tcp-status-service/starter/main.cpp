#include "academy/net/socket.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_tcp_line_server([&](const std::string& request) {
        // TODO: 支持 STATUS\n 和 TOGGLE\n；返回 reminder=ON\n 或 reminder=OFF\n。
        (void)request;
        return std::string("TODO\n");
    });
}
