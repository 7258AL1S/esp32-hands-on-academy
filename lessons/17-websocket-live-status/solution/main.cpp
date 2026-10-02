#include "academy/net/websocket.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_websocket_server([&](const std::string& message) {
        if (message == "status") {
            return std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}");
        }
        if (message == "toggle") {
            reminder_on = !reminder_on;
            return std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}");
        }
        return std::string("{\"error\":\"unknown command\"}");
    });
}
