#include "academy/net/websocket.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_websocket_server([&](const std::string& message) {
        // TODO: status 返回当前 JSON；toggle 翻转并返回更新后的 JSON。
        (void)message;
        (void)reminder_on;
        return std::string("{\"error\":\"TODO\"}");
    });
}
