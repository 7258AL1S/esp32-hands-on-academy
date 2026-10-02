#include "esp32_board.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    academy::Esp32Board board(GPIO_NUM_19, GPIO_NUM_18);
    ESP_ERROR_CHECK(board.init());
    while (true) {
        // Guided level-following only. Your edge/debounce Controller comes next.
        board.write_led(board.read_button() == academy::Level::low);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
