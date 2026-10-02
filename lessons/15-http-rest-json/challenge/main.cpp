#include "academy/net/http.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_http_server([&](const academy::net::HttpRequest& request) {
        using academy::net::HttpResponse;
        if (request.method == "GET" && request.path == "/api/status") {
            return HttpResponse{200, "application/json", std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}")};
        }
        if (request.method == "POST" && request.path == "/api/reminder") {
            bool reminder_on = request.body == "{\"enabled\":true}"; // 遮蔽了设备状态。
            return HttpResponse{200, "application/json", reminder_on ? "{\"reminder\":true}" : "{\"reminder\":false}"};
        }
        return HttpResponse{404, "application/json", "{}"};
    });
}
