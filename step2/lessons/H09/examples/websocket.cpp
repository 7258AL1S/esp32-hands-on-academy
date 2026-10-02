#include "wifi_link.hpp"
#include "esp_http_server.h"
#include "esp_log.h"

static esp_err_t websocket(httpd_req_t* req) {
    if (req->method == HTTP_GET) {
        return ESP_OK; // WebSocket upgrade handshake.
    }
    httpd_ws_frame_t frame{};
    const esp_err_t header_result = httpd_ws_recv_frame(req, &frame, 0);
    if (header_result != ESP_OK) {
        return header_result;
    }
    if (frame.len > 64) {
        return ESP_ERR_INVALID_SIZE;
    }
    uint8_t payload[65]{};
    frame.payload = payload;
    const esp_err_t err = httpd_ws_recv_frame(req, &frame, sizeof(payload) - 1);
    if (err != ESP_OK) {
        return err;
    }
    // Guided echo only; a device-state broadcaster is the learner's challenge.
    return httpd_ws_send_frame(req, &frame);
}
extern "C" void app_main() {
    ESP_ERROR_CHECK(wifi_link_start());
    while (!wifi_link_wait(pdMS_TO_TICKS(1000))) {
    }
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));
    httpd_uri_t uri{};
    uri.uri = "/ws";
    uri.method = HTTP_GET;
    uri.handler = websocket;
    uri.is_websocket = true;
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &uri));
}
