#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    i2c_master_bus_config_t bus_cfg{};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = GPIO_NUM_21;
    bus_cfg.scl_io_num = GPIO_NUM_22;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    // Module/external pull-ups to 3.3 V are required; internal pull-ups are not a bus design.
    i2c_master_bus_handle_t bus = nullptr;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));
    i2c_device_config_t device_cfg{};
    device_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    device_cfg.device_address = 0x23;
    device_cfg.scl_speed_hz = 100000;
    i2c_master_dev_handle_t sensor = nullptr;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &device_cfg, &sensor));
    while (true) {
        // BH1750 continuous high-resolution mode. ADDR low -> 0x23.
        const uint8_t command = 0x10;
        esp_err_t err = i2c_master_transmit(sensor, &command, 1, 100);
        if (err == ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(180));
            uint8_t bytes[2]{};
            err = i2c_master_receive(sensor, bytes, 2, 100);
            if (err == ESP_OK) {
                const unsigned raw = (unsigned(bytes[0]) << 8) | bytes[1];
                ESP_LOGI("i2c", "BH1750 raw=%u lux_estimate=%.1f", raw, raw / 1.2);
            }
        }
        if (err != ESP_OK) {
            ESP_LOGW("i2c", "transaction failed: %s; retry in 1s",
                     esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
