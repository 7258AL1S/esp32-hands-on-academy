#include "academy/net/mqtt.hpp"
#include <iostream>

int main() {
    try {
        auto client = academy::net::MqttClient::connect(
            "127.0.0.1", academy::net::env_port("ACADEMY_MQTT_PORT"), "desk-reminder");
        client.publish("desk/reminder/state", "offline");
        client.subscribe("desk/reminder/cmd");
        const auto command = client.receive_publish();
        const bool reminder_on =
            command.topic == "desk/reminder/cmd" && command.payload == "toggle";
        client.publish("desk/reminder/state", reminder_on ? "on" : "off");
        std::cout << "state=" << (reminder_on ? "on" : "off") << '\n';
        return reminder_on ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
