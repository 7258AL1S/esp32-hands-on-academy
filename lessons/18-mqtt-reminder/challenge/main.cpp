#include "academy/net/mqtt.hpp"
#include <iostream>

int main() {
    try {
        auto client = academy::net::MqttClient::connect(
            "127.0.0.1", academy::net::env_port("ACADEMY_MQTT_PORT"), "desk-reminder");
        client.publish("desk/reminder/state", "offline");
        client.subscribe("desk/reminder/cmd");
        const auto command = client.receive_publish();
        if (command.payload == "toggle")
            client.publish("desk/reminder/states", "on"); // topic 拼错。
        std::cout << "state=on\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
