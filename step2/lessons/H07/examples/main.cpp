#include "wifi_link.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_ERROR_CHECK(wifi_link_start());
    while (true) {
        ESP_LOGI("status", "has_ip=%d", wifi_link_wait(pdMS_TO_TICKS(1000)));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
