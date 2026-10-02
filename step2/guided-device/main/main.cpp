#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_LOGI("desk_tool", "Empty project ready; add your first feature here.");
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
