#include "academy/net/mqtt.hpp"
#include <iostream>

int main() {
    try {
        auto client = academy::net::MqttClient::connect("127.0.0.1", academy::net::env_port("ACADEMY_MQTT_PORT"), "desk-reminder");
        // TODO: publish state=offline；subscribe desk/reminder/cmd；收到 toggle 后 publish state=on。
        (void)client;
        std::cout << "TODO\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
