#pragma once

#include "academy/net/socket.hpp"

#include <cstdint>

namespace academy::net {

struct MqttMessage {
    std::string topic;
    std::string payload;
};

inline void mqtt_append_length(std::string& output, std::size_t length) {
    do {
        std::uint8_t byte = static_cast<std::uint8_t>(length % 128);
        length /= 128;
        if (length > 0) byte |= 0x80;
        output.push_back(static_cast<char>(byte));
    } while (length > 0);
}

inline std::size_t mqtt_read_length(const Socket& socket) {
    std::size_t value = 0;
    std::size_t multiplier = 1;
    for (int count = 0; count < 4; ++count) {
        const std::string byte = receive_exact(socket, 1);
        const auto value_byte = static_cast<std::uint8_t>(byte[0]);
        value += (value_byte & 127) * multiplier;
        if ((value_byte & 128) == 0) return value;
        multiplier *= 128;
    }
    throw std::runtime_error("invalid MQTT remaining length");
}

inline void mqtt_append_string(std::string& output, std::string_view value) {
    if (value.size() > 65535) throw std::runtime_error("MQTT string too long");
    output.push_back(static_cast<char>((value.size() >> 8) & 0xff));
    output.push_back(static_cast<char>(value.size() & 0xff));
    output.append(value.data(), value.size());
}

inline std::string mqtt_read_string(std::string_view bytes, std::size_t& offset) {
    if (offset + 2 > bytes.size()) throw std::runtime_error("truncated MQTT string length");
    const std::size_t length = (static_cast<std::uint8_t>(bytes[offset]) << 8) |
                               static_cast<std::uint8_t>(bytes[offset + 1]);
    offset += 2;
    if (offset + length > bytes.size()) throw std::runtime_error("truncated MQTT string");
    std::string value(bytes.substr(offset, length));
    offset += length;
    return value;
}

inline void mqtt_send_packet(const Socket& socket, std::uint8_t header, std::string_view body) {
    std::string packet;
    packet.push_back(static_cast<char>(header));
    mqtt_append_length(packet, body.size());
    packet.append(body.data(), body.size());
    send_all(socket, packet);
}

inline std::pair<std::uint8_t, std::string> mqtt_receive_packet(const Socket& socket) {
    const std::string header = receive_exact(socket, 1);
    const std::size_t length = mqtt_read_length(socket);
    const std::string body = receive_exact(socket, length);
    return {static_cast<std::uint8_t>(header[0]), body};
}

class MqttClient {
public:
    static MqttClient connect(std::string_view host, std::uint16_t port, std::string_view client_id) {
        SocketSystem& system = socket_system();
        (void)system;
        MqttClient client(connect_tcp(host, port));
        std::string body;
        mqtt_append_string(body, "MQTT");
        body.push_back(4);       // MQTT 3.1.1
        body.push_back(0x02);    // clean session
        body.push_back(0); body.push_back(30);
        mqtt_append_string(body, client_id);
        mqtt_send_packet(client.socket_, 0x10, body);
        const auto [header, reply] = mqtt_receive_packet(client.socket_);
        if (header != 0x20 || reply.size() != 2 || static_cast<std::uint8_t>(reply[1]) != 0) {
            throw std::runtime_error("MQTT CONNACK rejected");
        }
        return client;
    }

    MqttClient(MqttClient&&) noexcept = default;
    MqttClient& operator=(MqttClient&&) noexcept = default;
    MqttClient(const MqttClient&) = delete;
    MqttClient& operator=(const MqttClient&) = delete;

    void subscribe(std::string_view topic, std::uint16_t packet_id = 1) {
        std::string body;
        body.push_back(static_cast<char>((packet_id >> 8) & 0xff));
        body.push_back(static_cast<char>(packet_id & 0xff));
        mqtt_append_string(body, topic);
        body.push_back(0); // QoS 0
        mqtt_send_packet(socket_, 0x82, body);
        const auto [header, reply] = mqtt_receive_packet(socket_);
        if (header != 0x90 || reply.size() < 3 || static_cast<std::uint8_t>(reply.back()) != 0) {
            throw std::runtime_error("MQTT SUBACK rejected");
        }
    }

    void publish(std::string_view topic, std::string_view payload) {
        std::string body;
        mqtt_append_string(body, topic);
        body.append(payload.data(), payload.size());
        mqtt_send_packet(socket_, 0x30, body);
    }

    MqttMessage receive_publish() {
        const auto [header, body] = mqtt_receive_packet(socket_);
        if ((header & 0xf0) != 0x30) throw std::runtime_error("expected MQTT PUBLISH");
        std::size_t offset = 0;
        MqttMessage message;
        message.topic = mqtt_read_string(body, offset);
        message.payload = body.substr(offset);
        return message;
    }

private:
    explicit MqttClient(Socket socket) : socket_(std::move(socket)) { set_timeout(socket_, 1800); }

    static SocketSystem& socket_system() {
        static SocketSystem instance;
        return instance;
    }

    Socket socket_;
};

} // namespace academy::net
