#include "wifi_link.hpp"
#include "esp_crt_bundle.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_ERROR_CHECK(wifi_link_start());
    while (!wifi_link_wait(pdMS_TO_TICKS(1000))) {
    }
    const esp_partition_t* running = esp_ota_get_running_partition();
    esp_ota_img_states_t state;
    if (esp_ota_get_state_partition(running, &state) == ESP_OK &&
        state == ESP_OTA_IMG_PENDING_VERIFY) {
        // Demo health test: obtaining IP. Challenge replaces this with product health criteria.
        ESP_ERROR_CHECK(esp_ota_mark_app_valid_cancel_rollback());
        ESP_LOGI("ota", "candidate confirmed by demo IP check");
    }
    ESP_LOGI("ota", "running=%s version=%s", running->label,
             esp_app_get_description()->version);
    esp_http_client_config_t http{};
    http.url = CONFIG_ACADEMY_OTA_URL;
    http.crt_bundle_attach = esp_crt_bundle_attach;
    http.timeout_ms = 15000;
    esp_https_ota_config_t ota{};
    ota.http_config = &http;
    const esp_err_t result = esp_https_ota(&ota);
    ESP_LOGI("ota", "update result=%s", esp_err_to_name(result));
    // Do not reboot into an endless update loop. Inspect result, then manually reset to test candidate.
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
