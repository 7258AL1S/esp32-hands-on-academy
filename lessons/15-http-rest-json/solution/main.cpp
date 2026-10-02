#include "academy/net/http.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_http_server([&](const academy::net::HttpRequest& request) {
        using academy::net::HttpResponse;
        if (request.method == "GET" && request.path == "/api/status") {
            return HttpResponse{200, "application/json; charset=utf-8",
                                std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}")};
        }
        if (request.method == "POST" && request.path == "/api/reminder") {
            if (request.body == "{\"enabled\":true}") reminder_on = true;
            else if (request.body == "{\"enabled\":false}") reminder_on = false;
            else return HttpResponse{400, "application/json; charset=utf-8", "{\"error\":\"enabled expected\"}"};
            return HttpResponse{200, "application/json; charset=utf-8",
                                std::string("{\"reminder\":") + (reminder_on ? "true}" : "false}")};
        }
        return HttpResponse{404, "application/json; charset=utf-8", "{\"error\":\"not found\"}"};
    });
}
