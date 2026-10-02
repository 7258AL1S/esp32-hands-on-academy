#include "wifi_link.hpp"
#include <cstring>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

static EventGroupHandle_t state;
static esp_timer_handle_t retry_timer;
static unsigned retries = 0;
static constexpr EventBits_t kHasIp = BIT0;
static void retry(void*) {
    const esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGW("wifi", "connect rejected: %s", esp_err_to_name(err));
    }
}
static void on_event(void*, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        retry(nullptr);
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupClearBits(state, kHasIp);
        const auto* event = static_cast<wifi_event_sta_disconnected_t*>(data);
        const unsigned seconds = 1U << (retries < 5 ? retries++ : 5);
        ESP_LOGW("wifi", "disconnected reason=%u; retry in %us", event->reason,
                 seconds);
        esp_timer_stop(retry_timer); // Already stopped is harmless here.
        ESP_ERROR_CHECK(esp_timer_start_once(retry_timer, uint64_t(seconds) * 1000000));
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        retries = 0;
        esp_timer_stop(retry_timer);
        auto* event = static_cast<ip_event_got_ip_t*>(data);
        ESP_LOGI("wifi", "IP=" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(state, kHasIp);
    }
}
esp_err_t wifi_link_start() {
    if (std::strlen(CONFIG_ACADEMY_WIFI_SSID) == 0) {
        ESP_LOGE("wifi",
                 "Set local Wi-Fi credentials in menuconfig; never commit them.");
        return ESP_ERR_INVALID_ARG;
    }
    // Do not automatically erase a user's NVS on initialization failure.
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    if (!esp_netif_create_default_wifi_sta()) {
        return ESP_ERR_NO_MEM;
    }
    state = xEventGroupCreate();
    if (!state) {
        return ESP_ERR_NO_MEM;
    }
    esp_timer_create_args_t timer{};
    timer.callback = retry;
    timer.name = "wifi_retry";
    ESP_ERROR_CHECK(esp_timer_create(&timer, &retry_timer));
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init));
    ESP_ERROR_CHECK(
        esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, on_event, nullptr));
    ESP_ERROR_CHECK(
        esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_event, nullptr));
    wifi_config_t cfg{};
    static_assert(sizeof(CONFIG_ACADEMY_WIFI_SSID) - 1 <= sizeof(cfg.sta.ssid));
    static_assert(sizeof(CONFIG_ACADEMY_WIFI_PASSWORD) - 1 <= sizeof(cfg.sta.password));
    std::memcpy(cfg.sta.ssid, CONFIG_ACADEMY_WIFI_SSID,
                sizeof(CONFIG_ACADEMY_WIFI_SSID) - 1);
    std::memcpy(cfg.sta.password, CONFIG_ACADEMY_WIFI_PASSWORD,
                sizeof(CONFIG_ACADEMY_WIFI_PASSWORD) - 1);
    cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    return esp_wifi_start();
}
bool wifi_link_wait(TickType_t timeout) {
    return state &&
           (xEventGroupWaitBits(state, kHasIp, pdFALSE, pdTRUE, timeout) & kHasIp);
}
