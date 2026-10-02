#pragma once

// Small, dependency-free localhost socket helpers for the STEP 1 labs.
// They are intentionally not an ESP32 networking implementation.

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifdef _WIN32
#  define NOMINMAX
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  pragma comment(lib, "ws2_32.lib")
#else
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

namespace academy::net {

#ifdef _WIN32
using socket_handle = SOCKET;
constexpr socket_handle invalid_socket = INVALID_SOCKET;
#else
using socket_handle = int;
constexpr socket_handle invalid_socket = -1;
#endif

inline std::string last_socket_error() {
#ifdef _WIN32
    return "socket error " + std::to_string(WSAGetLastError());
#else
    return std::strerror(errno);
#endif
}

class SocketSystem {
public:
    SocketSystem() {
#ifdef _WIN32
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
#endif
    }
    ~SocketSystem() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
    SocketSystem(const SocketSystem&) = delete;
    SocketSystem& operator=(const SocketSystem&) = delete;
};

inline void close_socket(socket_handle handle) {
    if (handle == invalid_socket) return;
#ifdef _WIN32
    closesocket(handle);
#else
    ::close(handle);
#endif
}

class Socket {
public:
    Socket() = default;
    explicit Socket(socket_handle handle) : handle_(handle) {}
    ~Socket() { close_socket(handle_); }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept : handle_(std::exchange(other.handle_, invalid_socket)) {}
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close_socket(handle_);
            handle_ = std::exchange(other.handle_, invalid_socket);
        }
        return *this;
    }
    [[nodiscard]] socket_handle native() const { return handle_; }
    [[nodiscard]] bool valid() const { return handle_ != invalid_socket; }
    socket_handle release() { return std::exchange(handle_, invalid_socket); }

private:
    socket_handle handle_ = invalid_socket;
};

inline void set_timeout(const Socket& socket, int milliseconds) {
#ifdef _WIN32
    DWORD value = static_cast<DWORD>(milliseconds);
    if (setsockopt(socket.native(), SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&value), sizeof(value)) != 0 ||
        setsockopt(socket.native(), SOL_SOCKET, SO_SNDTIMEO,
                   reinterpret_cast<const char*>(&value), sizeof(value)) != 0) {
        throw std::runtime_error(last_socket_error());
    }
#else
    timeval value{};
    value.tv_sec = milliseconds / 1000;
    value.tv_usec = (milliseconds % 1000) * 1000;
    if (setsockopt(socket.native(), SOL_SOCKET, SO_RCVTIMEO, &value, sizeof(value)) != 0 ||
        setsockopt(socket.native(), SOL_SOCKET, SO_SNDTIMEO, &value, sizeof(value)) != 0) {
        throw std::runtime_error(last_socket_error());
    }
#endif
}

inline Socket create_tcp_socket() {
    Socket socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (!socket.valid()) throw std::runtime_error("create TCP socket: " + last_socket_error());
    return socket;
}

inline Socket create_udp_socket() {
    Socket socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
    if (!socket.valid()) throw std::runtime_error("create UDP socket: " + last_socket_error());
    return socket;
}

inline sockaddr_in localhost_address(std::uint16_t port) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    return address;
}

inline std::uint16_t local_port(const Socket& socket) {
    sockaddr_in address{};
#ifdef _WIN32
    int length = sizeof(address);
#else
    socklen_t length = sizeof(address);
#endif
    if (getsockname(socket.native(), reinterpret_cast<sockaddr*>(&address), &length) != 0) {
        throw std::runtime_error("getsockname: " + last_socket_error());
    }
    return ntohs(address.sin_port);
}

inline Socket listen_localhost(std::uint16_t requested_port = 0) {
    Socket socket = create_tcp_socket();
    int yes = 1;
    setsockopt(socket.native(), SOL_SOCKET, SO_REUSEADDR,
#ifdef _WIN32
               reinterpret_cast<const char*>(&yes), sizeof(yes));
#else
               &yes, sizeof(yes));
#endif
    const auto address = localhost_address(requested_port);
    if (bind(socket.native(), reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
        throw std::runtime_error("bind localhost: " + last_socket_error());
    }
    if (listen(socket.native(), 8) != 0) {
        throw std::runtime_error("listen: " + last_socket_error());
    }
    return socket;
}

inline Socket bind_udp_localhost(std::uint16_t requested_port = 0) {
    Socket socket = create_udp_socket();
    const auto address = localhost_address(requested_port);
    if (bind(socket.native(), reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
        throw std::runtime_error("bind UDP localhost: " + last_socket_error());
    }
    return socket;
}

inline Socket accept_one(const Socket& listener) {
    sockaddr_in address{};
#ifdef _WIN32
    int length = sizeof(address);
#else
    socklen_t length = sizeof(address);
#endif
    Socket client(accept(listener.native(), reinterpret_cast<sockaddr*>(&address), &length));
    if (!client.valid()) throw std::runtime_error("accept: " + last_socket_error());
    return client;
}

inline Socket connect_tcp(std::string_view host, std::uint16_t port, int timeout_ms = 1200) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* results = nullptr;
    const std::string port_text = std::to_string(port);
    const std::string host_text(host);
    if (getaddrinfo(host_text.c_str(), port_text.c_str(), &hints, &results) != 0) {
        throw std::runtime_error("DNS resolve failed for " + host_text);
    }
    std::string error;
    for (auto* item = results; item != nullptr; item = item->ai_next) {
        Socket socket(::socket(item->ai_family, item->ai_socktype, item->ai_protocol));
        if (!socket.valid()) continue;
        set_timeout(socket, timeout_ms);
        if (connect(socket.native(), item->ai_addr,
#ifdef _WIN32
                    static_cast<int>(item->ai_addrlen)
#else
                    item->ai_addrlen
#endif
                    ) == 0) {
            freeaddrinfo(results);
            return socket;
        }
        error = last_socket_error();
    }
    freeaddrinfo(results);
    throw std::runtime_error("connect " + host_text + ": " + error);
}

inline void send_all(const Socket& socket, std::string_view text) {
    std::size_t offset = 0;
    while (offset < text.size()) {
#ifdef _WIN32
        const int sent = send(socket.native(), text.data() + offset,
                              static_cast<int>(text.size() - offset), 0);
#else
        const auto sent = send(socket.native(), text.data() + offset, text.size() - offset, 0);
#endif
        if (sent <= 0) throw std::runtime_error("send: " + last_socket_error());
        offset += static_cast<std::size_t>(sent);
    }
}

inline std::string receive_some(const Socket& socket, std::size_t max_bytes = 8192) {
    std::string output(max_bytes, '\0');
#ifdef _WIN32
    const int read = recv(socket.native(), output.data(), static_cast<int>(output.size()), 0);
#else
    const auto read = recv(socket.native(), output.data(), output.size(), 0);
#endif
    if (read < 0) throw std::runtime_error("receive: " + last_socket_error());
    output.resize(static_cast<std::size_t>(read));
    return output;
}

inline std::string receive_exact(const Socket& socket, std::size_t bytes) {
    std::string output;
    output.reserve(bytes);
    while (output.size() < bytes) {
        const std::string chunk = receive_some(socket, bytes - output.size());
        if (chunk.empty()) throw std::runtime_error("peer closed before expected bytes arrived");
        output += chunk;
    }
    return output;
}

inline std::string receive_until(const Socket& socket, std::string_view delimiter,
                                 std::size_t max_bytes = 65536) {
    std::string output;
    output.reserve(1024);
    while (output.find(delimiter) == std::string::npos && output.size() < max_bytes) {
        const std::string chunk = receive_some(socket, std::min<std::size_t>(2048, max_bytes - output.size()));
        if (chunk.empty()) break;
        output += chunk;
    }
    return output;
}

inline std::string request_line(std::string_view host, std::uint16_t port, std::string_view line) {
    Socket socket = connect_tcp(host, port);
    send_all(socket, line);
    return receive_until(socket, "\n");
}

inline std::uint16_t env_port(const char* name, std::uint16_t fallback = 0) {
    const char* value = std::getenv(name);
    if (!value || !*value) return fallback;
    const long number = std::strtol(value, nullptr, 10);
    if (number < 0 || number > 65535) throw std::runtime_error("invalid port environment value");
    return static_cast<std::uint16_t>(number);
}

inline bool env_flag(const char* name) {
    const char* value = std::getenv(name);
    return value && std::string_view(value) == "1";
}

inline std::string resolve_ipv4(std::string_view host) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* results = nullptr;
    const std::string host_text(host);
    if (getaddrinfo(host_text.c_str(), nullptr, &hints, &results) != 0) {
        throw std::runtime_error("DNS resolve failed for " + host_text);
    }
    char text[INET_ADDRSTRLEN]{};
    const auto* address = reinterpret_cast<const sockaddr_in*>(results->ai_addr);
    const char* value = inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text));
    freeaddrinfo(results);
    if (!value) throw std::runtime_error("inet_ntop failed");
    return value;
}

inline void announce_ready(std::string_view kind, std::uint16_t port) {
    std::cout << "ACADEMY_READY " << kind << " " << port << std::endl;
}

template <typename Handler>
int run_tcp_line_server(Handler handler, std::uint16_t requested_port = env_port("ACADEMY_PORT")) {
    SocketSystem system;
    Socket listener = listen_localhost(requested_port);
    announce_ready("tcp", local_port(listener));
    while (true) {
        try {
            Socket client = accept_one(listener);
            set_timeout(client, 1500);
            const std::string request = receive_until(client, "\n", 4096);
            send_all(client, handler(request));
        } catch (const std::exception&) {
            // A malformed or abandoned client must not stop the lesson server.
        }
    }
}

template <typename Handler>
int run_udp_server(Handler handler, std::uint16_t requested_port = env_port("ACADEMY_PORT")) {
    SocketSystem system;
    Socket socket = bind_udp_localhost(requested_port);
    announce_ready("udp", local_port(socket));
    while (true) {
        std::array<char, 2048> bytes{};
        sockaddr_in client{};
#ifdef _WIN32
        int length = sizeof(client);
        const int read = recvfrom(socket.native(), bytes.data(), static_cast<int>(bytes.size()), 0,
                                  reinterpret_cast<sockaddr*>(&client), &length);
#else
        socklen_t length = sizeof(client);
        const auto read = recvfrom(socket.native(), bytes.data(), bytes.size(), 0,
                                   reinterpret_cast<sockaddr*>(&client), &length);
#endif
        if (read <= 0) continue;
        const std::string response = handler(std::string_view(bytes.data(), static_cast<std::size_t>(read)));
        sendto(socket.native(), response.data(),
#ifdef _WIN32
               static_cast<int>(response.size()),
#else
               response.size(),
#endif
               0, reinterpret_cast<const sockaddr*>(&client), length);
    }
}

} // namespace academy::net
