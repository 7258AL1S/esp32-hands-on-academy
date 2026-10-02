#include "academy/net/http.hpp"

int main() {
    bool reminder_on = false;
    return academy::net::run_http_server([&](const academy::net::HttpRequest& request) {
        using academy::net::HttpResponse;
        // TODO: GET /api/status 返回 {"reminder":false/true}。
        // TODO: POST /api/reminder 接受 {"enabled":true/false} 并保存状态。
        (void)request;
        (void)reminder_on;
        return HttpResponse{404, "application/json; charset=utf-8", "{\"error\":\"TODO\"}"};
    });
}
