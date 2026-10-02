#include "status_policy.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_LOGI("policy", "ready=%d", status_policy::should_show_ready(true, false));
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
