#include <cstring>
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    // Disconnect any external SPI device; connect GPIO13(MOSI) to GPIO32(MISO).
    spi_bus_config_t bus{};
    bus.mosi_io_num = 13;
    bus.miso_io_num = 32;
    bus.sclk_io_num = 14;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_DISABLED));
    spi_device_interface_config_t cfg{};
    cfg.clock_speed_hz = 1000000;
    cfg.mode = 0;
    cfg.spics_io_num = 27;
    cfg.queue_size = 1;
    spi_device_handle_t device = nullptr;
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &cfg, &device));
    uint8_t tx[] = {0x12, 0x34, 0x56, 0x78};
    uint8_t rx[sizeof(tx)]{};
    spi_transaction_t transfer{};
    transfer.length = sizeof(tx) * 8; // bits, not bytes
    transfer.tx_buffer = tx;
    transfer.rx_buffer = rx;
    ESP_ERROR_CHECK(spi_device_transmit(device, &transfer));
    ESP_LOGI("spi", "loopback=%s",
             std::memcmp(tx, rx, sizeof(tx)) == 0 ? "PASS" : "FAIL");
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
