#include "wifi_link.hpp"
#include "mqtt_client.h"
#include "esp_log.h"
#include "sdkconfig.h"

static void event(void*, esp_event_base_t, int32_t id, void* raw) {
    auto* message = static_cast<esp_mqtt_event_t*>(raw);
    if (id == MQTT_EVENT_CONNECTED) {
        const int subscribed =
            esp_mqtt_client_subscribe(message->client, "academy/desk/cmd", 1);
        const int published = esp_mqtt_client_publish(
            message->client, "academy/desk/status", "online", 0, 1, 1);
        ESP_LOGI("mqtt", "connected subscribe_id=%d publish_id=%d", subscribed,
                 published);
    } else if (id == MQTT_EVENT_DATA) {
        // Fragments are NOT necessarily complete commands; never use strcmp(data,...).
        ESP_LOGI("mqtt", "chunk=%d offset=%d total=%d topic=%.*s data=%.*s",
                 message->data_len, message->current_data_offset,
                 message->total_data_len, message->topic_len,
                 message->topic ? message->topic : "", message->data_len,
                 message->data);
    } else if (id == MQTT_EVENT_DISCONNECTED) {
        ESP_LOGW("mqtt", "broker disconnected; client will reconnect");
    }
}
extern "C" void app_main() {
    ESP_ERROR_CHECK(wifi_link_start());
    while (!wifi_link_wait(pdMS_TO_TICKS(1000))) {
    }
    esp_mqtt_client_config_t cfg{};
    cfg.broker.address.uri = CONFIG_ACADEMY_MQTT_URI;
    cfg.session.last_will.topic = "academy/desk/status";
    cfg.session.last_will.msg = "offline";
    cfg.session.last_will.qos = 1;
    cfg.session.last_will.retain = 1;
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&cfg);
    if (!client) {
        ESP_LOGE("mqtt", "client allocation failed");
        return;
    }
    ESP_ERROR_CHECK(
        esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, event, nullptr));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
}
