#pragma once
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
esp_err_t wifi_link_start();
bool wifi_link_wait(TickType_t timeout);
