#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_ERROR_CHECK(
        nvs_flash_init()); // Stop and diagnose; never silently erase all namespaces.
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open("desk", NVS_READWRITE, &handle));
    uint32_t boots = 0;
    const esp_err_t result = nvs_get_u32(handle, "boots", &boots);
    if (result != ESP_OK && result != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        ESP_ERROR_CHECK(result);
        return;
    }
    if (boots == UINT32_MAX) {
        ESP_LOGE("nvs", "counter full; reset explicitly instead of wrapping");
        nvs_close(handle);
        return;
    }
    ++boots;
    ESP_ERROR_CHECK(nvs_set_u32(handle, "boots", boots));
    ESP_ERROR_CHECK(nvs_commit(handle));
    nvs_close(handle);
    ESP_LOGI("nvs", "boots=%lu", (unsigned long)boots);
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
