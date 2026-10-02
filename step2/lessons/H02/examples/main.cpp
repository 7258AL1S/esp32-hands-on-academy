#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "device_name.hpp"

extern "C" void app_main() {
    ESP_LOGI("desk_tool", "device=%s", device_name());
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
