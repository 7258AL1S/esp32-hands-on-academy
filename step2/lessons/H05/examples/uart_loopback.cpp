#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    uart_config_t cfg{};
    cfg.baud_rate = 115200;
    cfg.data_bits = UART_DATA_8_BITS;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    cfg.source_clk = UART_SCLK_DEFAULT;
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &cfg));
    ESP_ERROR_CHECK(
        uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 256, 0, 0, nullptr, 0));
    const char message[] = "STATUS\n";
    uint8_t data[32];
    while (true) {
        const int sent = uart_write_bytes(UART_NUM_2, message, sizeof(message) - 1);
        const int received =
            uart_read_bytes(UART_NUM_2, data, sizeof(data), pdMS_TO_TICKS(200));
        if (received > 0) {
            ESP_LOGI("uart", "sent=%d chunk=%d data=%.*s", sent, received, received,
                     data);
        } else {
            ESP_LOGW("uart", "sent=%d no data (timeout/error=%d)", sent, received);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
