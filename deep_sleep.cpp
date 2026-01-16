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
        default : ESP_LOGI(tag.c_str(), "Wakeup was not caused by deep sleep: %d",wakeup_reason); break;
    }

}

DeepSleep::~DeepSleep() {}

bool DeepSleep::GoToDeepSleep( unsigned long sleepTime,
                               std::string sleepTimeUnit // {"min", "sec", "milliSec", "microSec"}
                             )
{
    // calculate timerIntervall
    unsigned long timerIntervall = sleepTime;
    if (sleepTimeUnit == std::string("min")) timerIntervall = timerIntervall * 60;
    if (sleepTimeUnit == std::string("min") ||
        sleepTimeUnit == std::string("sec")) timerIntervall = timerIntervall * 1000;
    if (sleepTimeUnit == std::string("min") ||
        sleepTimeUnit == std::string("sec") ||
        sleepTimeUnit == std::string("milliSec")) timerIntervall = timerIntervall * 1000;

    //***** // deep sleep aktivieren
    //***** WiFi.disconnect();

    //***** //Serial.println("Execution Time: " + String(millis());
    ESP_LOGI(tag.c_str(), "Setup ESP32 to sleep until button pressed or for %lu %s", sleepTime, sleepTimeUnit.c_str());
    esp_sleep_enable_timer_wakeup(timerIntervall);

    //***** #if defined(M5_ATOM_LITE)
    //*****   // M5_ATOM_LITE: wir benutzen ext0_wakeup mit dem Button
    //*****   esp_sleep_enable_ext0_wakeup((gpio_num_t) buttonPin, 0);  //1 = High, 0 = Low
    //*****   rtc_gpio_pullup_en((gpio_num_t) buttonPin);
    //*****   rtc_gpio_pulldown_dis((gpio_num_t) buttonPin);
    //***** #elif defined(ESP32_C6_MINI) || defined(ESP_C3_MINI)
    //*****   // ESPC6Mini: wir benutzen ext1_wakeup mit einem externen Button an GPIO 2,
    //*****   // weil der Boot-Button im Deep Sleep Modus nicht geht
    //*****
    //*****   esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    //*****
    //*****   // Configure GPIO02 as RTC IO for EXT1 wake-up
    //*****   //rtc_gpio_deinit((gpio_num_t) buttonPin);
    //*****
    //*****   // Enable EXT1 wake-up source
    //*****   esp_sleep_enable_ext1_wakeup_io((1ULL << buttonPin), ESP_EXT1_WAKEUP_ANY_HIGH);
    //*****
    //*****   // enable pull-down resistors and disable pull-up resistors
    //*****   rtc_gpio_pulldown_en((gpio_num_t) buttonPin);
    //*****   rtc_gpio_pullup_dis((gpio_num_t) buttonPin);
    //***** #endif

    ESP_LOGI(tag.c_str(), "Going to sleep now");
    //***** Serial.flush();
    esp_deep_sleep_start();

    // this statement will only be reached, if deep sleep start did not work!
    return false;
}