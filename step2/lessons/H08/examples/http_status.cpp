#include "wifi_link.hpp"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdio>

static esp_err_t status(httpd_req_t* req) {
    char json[128];
    std::snprintf(json, sizeof(json), "{\"uptime_ms\":%lld,\"status\":\"ready\"}",
                  (long long)(esp_timer_get_time() / 1000));
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}
static esp_err_t home(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(
        req,
        "<!doctype html><meta charset='utf-8'><style>body{background:#1e1e1e;"
        "color:#eee;font:16px/1.6 sans-serif;max-width:44rem;margin:2rem auto;"
        "padding:1rem;overflow-wrap:anywhere}a{color:#8cc8ff}</style>"
        "<h1>ESP32 Desk Tool</h1><p>真实设备状态：<a href='/api/status'>"
        "/api/status</a></p>",
        HTTPD_RESP_USE_STRLEN);
}
extern "C" void app_main() {
    ESP_ERROR_CHECK(wifi_link_start());
    while (!wifi_link_wait(pdMS_TO_TICKS(1000))) {
        ESP_LOGI("http", "waiting for IP");
    }
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    httpd_uri_t api{};
    api.uri = "/api/status";
    api.method = HTTP_GET;
    api.handler = status;
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &api));
    httpd_uri_t page{};
    page.uri = "/";
    page.method = HTTP_GET;
    page.handler = home;
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &page));
}
