#include <cinttypes>
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t owner = nullptr;
static void on_timer(void*) {
    // Default esp_timer callback runs in a task, not in an ISR.
    xTaskNotifyGive(owner);
}

extern "C" void app_main() {
    owner = xTaskGetCurrentTaskHandle();
    esp_timer_create_args_t args{};
    args.callback = on_timer;
    args.name = "heartbeat";
    esp_timer_handle_t timer = nullptr;
    ESP_ERROR_CHECK(esp_timer_create(&args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 1000000));
    int64_t previous = esp_timer_get_time();
    while (true) {
        const uint32_t pending = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(20));
        if (pending > 0) {
            const int64_t now = esp_timer_get_time();
            ESP_LOGI("timer", "pending=%" PRIu32 " dt_us=%" PRId64, pending,
                     now - previous);
            previous = now;
        }
        // Insert one short Controller tick here; never wait for the whole countdown.
    }
}
