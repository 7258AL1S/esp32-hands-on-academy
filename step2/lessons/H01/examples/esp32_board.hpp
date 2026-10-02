#pragma once
#include "board.hpp"
#include "driver/gpio.h"

namespace academy {

    class Esp32Board final : public Board {
    public:
        Esp32Board(gpio_num_t button, gpio_num_t led) : button_(button), led_(led) {
        }

        esp_err_t init() {
            gpio_config_t output{};
            output.pin_bit_mask = 1ULL << led_;
            output.mode = GPIO_MODE_OUTPUT;
            esp_err_t result = gpio_config(&output);
            if (result != ESP_OK) {
                return result;
            }
            result = gpio_set_level(led_, 0);
            if (result != ESP_OK) {
                return result;
            }
            gpio_config_t input{};
            input.pin_bit_mask = 1ULL << button_;
            input.mode = GPIO_MODE_INPUT;
            input.pull_up_en = GPIO_PULLUP_ENABLE;
            return gpio_config(&input);
        }
        Level read_button() const override {
            return gpio_get_level(button_) == 0 ? Level::low : Level::high;
        }
        void write_led(bool on) override {
            // Same void contract as STEP 1; configuration errors are checked by init.
            ESP_ERROR_CHECK(gpio_set_level(led_, on ? 1 : 0));
        }

    private:
        gpio_num_t button_;
        gpio_num_t led_;
    };

} // namespace academy
