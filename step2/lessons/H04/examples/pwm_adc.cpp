#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ledc_timer_config_t timer{};
    timer.speed_mode = LEDC_LOW_SPEED_MODE;
    timer.duty_resolution = LEDC_TIMER_10_BIT;
    timer.timer_num = LEDC_TIMER_0;
    timer.freq_hz = 5000;
    timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&timer));
    ledc_channel_config_t channel{};
    channel.gpio_num = 18;
    channel.speed_mode = LEDC_LOW_SPEED_MODE;
    channel.channel = LEDC_CHANNEL_0;
    channel.timer_sel = LEDC_TIMER_0;
    ESP_ERROR_CHECK(ledc_channel_config(&channel));
    adc_oneshot_unit_init_cfg_t unit_cfg{};
    unit_cfg.unit_id = ADC_UNIT_1;
    adc_oneshot_unit_handle_t adc = nullptr;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc));
    adc_oneshot_chan_cfg_t adc_cfg{};
    adc_cfg.atten = ADC_ATTEN_DB_12;
    adc_cfg.bitwidth = ADC_BITWIDTH_12;
    // Classic ESP32 GPIO34 = ADC1 channel 6. Not portable to other chips.
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL_6, &adc_cfg));
    while (true) {
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc, ADC_CHANNEL_6, &raw));
        const uint32_t duty = static_cast<uint32_t>(raw) * 1023 / 4095;
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
        ESP_LOGI("measure", "raw=%d duty=%lu/1023 (not calibrated mV)", raw,
                 (unsigned long)duty);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
