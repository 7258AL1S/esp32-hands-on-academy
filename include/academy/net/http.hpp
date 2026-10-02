#pragma once

#include "academy/net/socket.hpp"

#include <cctype>
#include <map>
#include <sstream>

namespace academy::net {

struct HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status = 200;
    std::string content_type = "application/json; charset=utf-8";
    std::string body = "{}";
};

inline std::string http_status_text(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        default: return "Internal Server Error";
    }
}

inline std::string trim_http(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) value.erase(value.begin());
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) value.pop_back();
    return value;
}

inline HttpRequest parse_http_request(const Socket& socket) {
    std::string raw = receive_until(socket, "\r\n\r\n");
    const auto header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos) throw std::runtime_error("incomplete HTTP headers");
    std::istringstream rows(raw.substr(0, header_end));
    std::string line;
    HttpRequest request;
    if (!std::getline(rows, line)) throw std::runtime_error("missing HTTP request line");
    if (!line.empty() && line.back() == '\r') line.pop_back();
    std::istringstream first(line);
    first >> request.method >> request.path;
    if (request.method.empty() || request.path.empty()) throw std::runtime_error("invalid HTTP request line");
    while (std::getline(rows, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto colon = line.find(':');
        if (colon != std::string::npos) {
            request.headers[trim_http(line.substr(0, colon))] = trim_http(line.substr(colon + 1));
        }
    }
    std::size_t length = 0;
    if (const auto found = request.headers.find("Content-Length"); found != request.headers.end()) {
        length = static_cast<std::size_t>(std::stoul(found->second));
    }
    request.body = raw.substr(header_end + 4);
    while (request.body.size() < length) {
        const std::string chunk = receive_some(socket, length - request.body.size());
        if (chunk.empty()) break;
        request.body += chunk;
    }
    return request;
}

inline std::string serialize_http_response(const HttpResponse& response) {
    std::ostringstream output;
    output << "HTTP/1.1 " << response.status << ' ' << http_status_text(response.status) << "\r\n"
           << "Content-Type: " << response.content_type << "\r\n"
           << "Content-Length: " << response.body.size() << "\r\n"
           << "Connection: close\r\n\r\n"
           << response.body;
    return output.str();
}

template <typename Handler>
int run_http_server(Handler handler, std::uint16_t requested_port = env_port("ACADEMY_PORT")) {
    SocketSystem system;
    Socket listener = listen_localhost(requested_port);
    announce_ready("http", local_port(listener));
    while (true) {
        try {
            Socket client = accept_one(listener);
            set_timeout(client, 1500);
            const HttpRequest request = parse_http_request(client);
            send_all(client, serialize_http_response(handler(request)));
        } catch (const std::exception&) {
            // Invalid requests are part of the debugging surface; keep serving the next client.
        }
    }
}

} // namespace academy::net
