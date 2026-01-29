/*
 * deep_sleep.cpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string>

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_idf_version.h"

#include "deep_sleep.hpp"

DeepSleep::DeepSleep( std::string tag,
                      int *bootCount
                    ) {
	this->tag = tag;

    // Increment boot number and print it every reboot
    ++(*bootCount);
    ESP_LOGI(tag.c_str(), "bootCount is %i", *bootCount);

    // find wakeup reason
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch(wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0 : ESP_LOGI(tag.c_str(), "Wakeup caused by external signal using RTC_IO"); break;
        case ESP_SLEEP_WAKEUP_EXT1 : ESP_LOGI(tag.c_str(), "Wakeup caused by external signal using RTC_CNTL"); break;
        case ESP_SLEEP_WAKEUP_TIMER : ESP_LOGI(tag.c_str(), "Wakeup caused by timer"); break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD : ESP_LOGI(tag.c_str(), "Wakeup caused by touchpad"); break;
        case ESP_SLEEP_WAKEUP_ULP : ESP_LOGI(tag.c_str(), "Wakeup caused by ULP program"); break;
        case ESP_SLEEP_WAKEUP_GPIO : ESP_LOGI(tag.c_str(), "Wakeup caused by gpio"); break;
        default : ESP_LOGI(tag.c_str(), "Wakeup was not caused by deep sleep: %d",wakeup_reason); break;
    }

    if (ESP_SLEEP_WAKEUP_EXT1 == wakeup_reason || ESP_SLEEP_WAKEUP_GPIO == wakeup_reason) {
        int GPIO_reason = esp_sleep_get_gpio_wakeup_status();
        int gpio = log10(GPIO_reason)/log10(2);
        ESP_LOGI(tag.c_str(), "GPIO that triggered the wake up: GPIO %i", gpio);
    }
}

DeepSleep::~DeepSleep() {}

esp_err_t DeepSleep::EnableTimerWakeup( unsigned long sleepTime,
                                        std::string sleepTimeUnit) // {"min", "sec", "milliSec", "microSec"}
{
    // calculate timerIntervall
    unsigned long timerIntervall = sleepTime;
    if (sleepTimeUnit == std::string("min")) timerIntervall = timerIntervall * 60;
    if (sleepTimeUnit == std::string("min") ||
        sleepTimeUnit == std::string("sec")) timerIntervall = timerIntervall * 1000;
    if (sleepTimeUnit == std::string("min") ||
        sleepTimeUnit == std::string("sec") ||
        sleepTimeUnit == std::string("milliSec")) timerIntervall = timerIntervall * 1000;

    ESP_LOGI(tag.c_str(), "Setup chip to sleep for %lu %s", sleepTime, sleepTimeUnit.c_str());
    return esp_sleep_enable_timer_wakeup(timerIntervall);
}

esp_err_t DeepSleep::EnableGpioWakeup( gpio_num_t gpio, int level) // level: 1 = High, 0 = Low)
{
    esp_err_t rc;
    bool valid;

    valid = esp_sleep_is_valid_wakeup_gpio(gpio);
    ESP_LOGI(tag.c_str(), "GPIO %u is valid wakeup gpio: %s", gpio, valid ? "true" : "false");

    ESP_LOGI(tag.c_str(), "gpio_config");
    if (level == 1) {
        const gpio_config_t config = {
            .pin_bit_mask = (1ULL << gpio),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = (gpio_pullup_t) 0,
            .pull_down_en = (gpio_pulldown_t) 1,
            .intr_type = GPIO_INTR_DISABLE,
        };
        rc = gpio_config(&config);
        if (rc != ESP_OK) {
            ESP_LOGI(tag.c_str(), "rc=%u", rc);
            return rc;
        }
    }
    else {
        const gpio_config_t config = {
            .pin_bit_mask = (1ULL << gpio),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = (gpio_pullup_t) 1,
            .pull_down_en = (gpio_pulldown_t) 0,
            .intr_type = GPIO_INTR_DISABLE,
        };
        rc = gpio_config(&config);
        if (rc != ESP_OK) {
            ESP_LOGI(tag.c_str(), "rc=%u", rc);
            return rc;
        }
    }

    gpio_dump_io_configuration(stdout, (1ULL << gpio));

    ESP_LOGI(tag.c_str(), "esp_deep_sleep_enable_gpio_wakeup");
    rc = esp_deep_sleep_enable_gpio_wakeup( (1ULL << gpio),
                                            (level ? ESP_GPIO_WAKEUP_GPIO_HIGH : ESP_GPIO_WAKEUP_GPIO_LOW)
                                          );
    if (rc != ESP_OK) {
        ESP_LOGI(tag.c_str(), "rc=%u", rc);
        return rc;
    }
    return rc;
}

esp_err_t DeepSleep::GoToDeepSleep()
{
    ESP_LOGI(tag.c_str(), "Going to sleep now");

    esp_err_t rc;
    rc = esp_deep_sleep_try_to_start();

    // this statement will only be reached, if esp_deep_sleep_try_to_start did not work!
    ESP_LOGI(tag.c_str(), "rc of esp_deep_sleep_try_to_start: %u", rc);

    return rc;
}