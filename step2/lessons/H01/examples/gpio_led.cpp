#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// GPIO labels, not physical header positions. Classic ESP32 external LED/button.
constexpr gpio_num_t kLed = GPIO_NUM_18;
constexpr gpio_num_t kButton = GPIO_NUM_19;

extern "C" void app_main() {
    gpio_config_t output{};
    output.pin_bit_mask = 1ULL << kLed;
    output.mode = GPIO_MODE_OUTPUT;
    ESP_ERROR_CHECK(gpio_config(&output));
    ESP_ERROR_CHECK(gpio_set_level(kLed, 0));
    gpio_config_t input{};
    input.pin_bit_mask = 1ULL << kButton;
    input.mode = GPIO_MODE_INPUT;
    input.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&input));
    int previous = -1;
    while (true) {
        const int raw = gpio_get_level(kButton);
        const bool pressed = raw == 0;
        ESP_ERROR_CHECK(gpio_set_level(kLed, pressed ? 1 : 0));
        if (raw != previous) {
            ESP_LOGI("gpio", "raw=%d pressed=%d output=%d", raw, pressed, pressed);
            previous = raw;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
