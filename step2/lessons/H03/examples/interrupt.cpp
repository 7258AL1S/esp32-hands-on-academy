#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t owner;
static void IRAM_ATTR on_edge(void*) {
    BaseType_t woken = pdFALSE;
    vTaskNotifyGiveFromISR(owner, &woken);
    if (woken) {
        portYIELD_FROM_ISR();
    }
}

extern "C" void app_main() {
    owner = xTaskGetCurrentTaskHandle();
    gpio_config_t input{};
    input.pin_bit_mask = 1ULL << GPIO_NUM_19;
    input.mode = GPIO_MODE_INPUT;
    input.pull_up_en = GPIO_PULLUP_ENABLE;
    input.intr_type = GPIO_INTR_ANYEDGE;
    ESP_ERROR_CHECK(gpio_config(&input));
    // No ESP_INTR_FLAG_IRAM: handler annotation alone does not make the entire call path IRAM-safe.
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_NUM_19, on_edge, nullptr));
    while (true) {
        const uint32_t edges = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
        if (edges > 0) {
            ESP_LOGI("edge", "edges=%lu raw=%d", (unsigned long)edges,
                     gpio_get_level(GPIO_NUM_19));
        }
    }
}
