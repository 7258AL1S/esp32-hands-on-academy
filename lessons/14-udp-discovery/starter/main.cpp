#include "academy/net/socket.hpp"

int main() {
    return academy::net::run_udp_server([](std::string_view packet) {
        // TODO: DISCOVER 必须回复 DESK_REMINDER|ONLINE，不能多换行或额外字段。
        (void)packet;
        return std::string("TODO");
    });
}
