#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void event(void*, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_PROV_EVENT && id == WIFI_PROV_CRED_SUCCESS) {
        ESP_LOGI("provision", "credentials accepted (never log password)");
    } else if (base == WIFI_PROV_EVENT && id == WIFI_PROV_END) {
        wifi_prov_mgr_deinit();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW("provision", "disconnected; add your H07 backoff policy here");
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        auto* ip = static_cast<ip_event_got_ip_t*>(data);
        ESP_LOGI("provision", "IP=" IPSTR, IP2STR(&ip->ip_info.ip));
    }
}
extern "C" void app_main() {
    // This demonstrates replacement of credential setup, not a second Wi-Fi stack.
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi));
    ESP_ERROR_CHECK(
        esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, event, nullptr));
    ESP_ERROR_CHECK(
        esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event, nullptr));
    ESP_ERROR_CHECK(
        esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event, nullptr));
    wifi_prov_mgr_config_t config{};
    config.scheme = wifi_prov_scheme_ble;
    config.scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM;
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    if (!provisioned) {
        // Teaching PoP only: replace per device before deployment; do not claim production security.
        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(
            WIFI_PROV_SECURITY_1, "academy-demo", "ACADEMY_DESK", nullptr));
        ESP_LOGI("provision", "Use Espressif provisioning app: BLE, name=ACADEMY_DESK");
    } else {
        wifi_prov_mgr_deinit();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
