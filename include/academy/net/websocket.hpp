#pragma once

#include "academy/net/http.hpp"

#include <array>
#include <iomanip>

namespace academy::net {

inline std::uint32_t rol(std::uint32_t value, int bits) {
    return (value << bits) | (value >> (32 - bits));
}

inline std::array<std::uint8_t, 20> sha1(std::string_view input) {
    std::vector<std::uint8_t> data(input.begin(), input.end());
    const std::uint64_t bit_length = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while ((data.size() % 64) != 56) data.push_back(0);
    for (int shift = 56; shift >= 0; shift -= 8) data.push_back(static_cast<std::uint8_t>(bit_length >> shift));
    std::uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;
    for (std::size_t offset = 0; offset < data.size(); offset += 64) {
        std::array<std::uint32_t, 80> words{};
        for (int i = 0; i < 16; ++i) {
            words[i] = (static_cast<std::uint32_t>(data[offset + i * 4]) << 24) |
                       (static_cast<std::uint32_t>(data[offset + i * 4 + 1]) << 16) |
                       (static_cast<std::uint32_t>(data[offset + i * 4 + 2]) << 8) |
                       data[offset + i * 4 + 3];
        }
        for (int i = 16; i < 80; ++i) words[i] = rol(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);
        std::uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; ++i) {
            const std::uint32_t f = i < 20 ? ((b & c) | ((~b) & d)) :
                                    i < 40 ? (b ^ c ^ d) :
                                    i < 60 ? ((b & c) | (b & d) | (c & d)) : (b ^ c ^ d);
            const std::uint32_t k = i < 20 ? 0x5A827999 : i < 40 ? 0x6ED9EBA1 :
                                    i < 60 ? 0x8F1BBCDC : 0xCA62C1D6;
            const std::uint32_t next = rol(a, 5) + f + e + k + words[i];
            e = d; d = c; c = rol(b, 30); b = a; a = next;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    std::array<std::uint8_t, 20> digest{};
    const std::array<std::uint32_t, 5> hashes{h0, h1, h2, h3, h4};
    for (std::size_t i = 0; i < hashes.size(); ++i) {
        digest[i * 4] = static_cast<std::uint8_t>(hashes[i] >> 24);
        digest[i * 4 + 1] = static_cast<std::uint8_t>(hashes[i] >> 16);
        digest[i * 4 + 2] = static_cast<std::uint8_t>(hashes[i] >> 8);
        digest[i * 4 + 3] = static_cast<std::uint8_t>(hashes[i]);
    }
    return digest;
}

inline std::string base64(const std::uint8_t* bytes, std::size_t length) {
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    output.reserve(((length + 2) / 3) * 4);
    for (std::size_t i = 0; i < length; i += 3) {
        const std::uint32_t group = static_cast<std::uint32_t>(bytes[i]) << 16 |
            (i + 1 < length ? static_cast<std::uint32_t>(bytes[i + 1]) << 8 : 0) |
            (i + 2 < length ? bytes[i + 2] : 0);
        output.push_back(alphabet[(group >> 18) & 63]);
        output.push_back(alphabet[(group >> 12) & 63]);
        output.push_back(i + 1 < length ? alphabet[(group >> 6) & 63] : '=');
        output.push_back(i + 2 < length ? alphabet[group & 63] : '=');
    }
    return output;
}

inline std::string websocket_accept(std::string_view key) {
    const auto digest = sha1(std::string(key) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    return base64(digest.data(), digest.size());
}

inline std::optional<std::string> receive_websocket_text(const Socket& socket) {
    const std::string prefix = receive_exact(socket, 2);
    const std::uint8_t opcode = static_cast<std::uint8_t>(prefix[0]) & 0x0f;
    std::uint64_t length = static_cast<std::uint8_t>(prefix[1]) & 0x7f;
    const bool masked = (static_cast<std::uint8_t>(prefix[1]) & 0x80) != 0;
    if (length == 126) {
        const std::string extra = receive_exact(socket, 2);
        length = (static_cast<std::uint8_t>(extra[0]) << 8) | static_cast<std::uint8_t>(extra[1]);
    }
    if (length > 65535 || !masked) throw std::runtime_error("unsupported WebSocket frame");
    const std::string key = receive_exact(socket, 4);
    std::string payload = receive_exact(socket, static_cast<std::size_t>(length));
    for (std::size_t i = 0; i < payload.size(); ++i) payload[i] ^= key[i % 4];
    if (opcode == 0x8) return std::nullopt;
    if (opcode != 0x1) throw std::runtime_error("only text WebSocket frames are used in this lesson");
    return payload;
}

inline void send_websocket_text(const Socket& socket, std::string_view payload) {
    if (payload.size() > 65535) throw std::runtime_error("WebSocket payload too large");
    std::string frame;
    frame.push_back(static_cast<char>(0x81));
    if (payload.size() < 126) {
        frame.push_back(static_cast<char>(payload.size()));
    } else {
        frame.push_back(126);
        frame.push_back(static_cast<char>((payload.size() >> 8) & 0xff));
        frame.push_back(static_cast<char>(payload.size() & 0xff));
    }
    frame.append(payload.data(), payload.size());
    send_all(socket, frame);
}

template <typename Handler>
int run_websocket_server(Handler handler, std::uint16_t requested_port = env_port("ACADEMY_PORT")) {
    SocketSystem system;
    Socket listener = listen_localhost(requested_port);
    announce_ready("ws", local_port(listener));
    while (true) {
        try {
            Socket client = accept_one(listener);
            set_timeout(client, 1500);
            const HttpRequest request = parse_http_request(client);
            const auto key = request.headers.find("Sec-WebSocket-Key");
            if (key == request.headers.end()) throw std::runtime_error("missing WebSocket key");
            send_all(client, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " +
                                 websocket_accept(key->second) + "\r\n\r\n");
            while (const auto message = receive_websocket_text(client)) {
                send_websocket_text(client, handler(*message));
            }
        } catch (const std::exception&) {
            // A bad handshake/frame should not end a student server's whole session.
        }
    }
}

} // namespace academy::net
