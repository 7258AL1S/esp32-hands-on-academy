#include <cstdint>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

struct Sample {
    uint32_t sequence;
    int value;
};
static QueueHandle_t samples;
static void producer(void*) {
    uint32_t sequence = 0;
    while (true) {
        Sample sample{sequence++, 42};
        if (xQueueSend(samples, &sample, pdMS_TO_TICKS(20)) != pdTRUE) {
            ESP_LOGW("queue", "full: dropped sequence=%lu",
                     (unsigned long)sample.sequence);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
extern "C" void app_main() {
    samples = xQueueCreate(4, sizeof(Sample));
    if (!samples) {
        ESP_LOGE("queue", "allocation failed");
        return;
    }
    if (xTaskCreate(producer, "sampler", 3072, nullptr, 5, nullptr) != pdPASS) {
        vQueueDelete(samples);
        ESP_LOGE("queue", "task creation failed");
        return;
    }
    Sample received{};
    while (true) {
        if (xQueueReceive(samples, &received, pdMS_TO_TICKS(500)) == pdTRUE) {
            ESP_LOGI("queue", "sequence=%lu value=%d stack_min_bytes=%u",
                     (unsigned long)received.sequence, received.value,
                     (unsigned)uxTaskGetStackHighWaterMark(nullptr));
        }
    }
}
