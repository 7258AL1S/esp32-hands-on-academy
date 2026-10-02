#include "academy/net/websocket.hpp"

int main() {
    return academy::net::run_websocket_server([](const std::string& message) {
        bool reminder_on = false; // 每一帧都重置，不能保存设备状态。
        if (message == "toggle") reminder_on = true;
        return std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}");
    });
}
