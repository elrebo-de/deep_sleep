/*
 * Example program to use deep_sleep functionality with elrebo-de/deep_sleep
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/rtc_io.h"
RTC_DATA_ATTR int bootCount = 0;

#include "deep_sleep.hpp"

#include "esp_log.h"

static const char *tag = "deep sleep test";

extern "C" void app_main(void)
{
    ESP_LOGI(tag, "Example Program");

    /* Initialize DeepSleep class */
    ESP_LOGI(tag, "DeepSleep");
    DeepSleep deepSleep(
		std::string("DeepSleep"), // tag
		&bootCount // Address of int bootCount in RTC_DATA
    );

    bool rc = false;

    switch(bootCount) {
        case 1:
            vTaskDelay(pdMS_TO_TICKS(5000)); // delay 5 seconds
            rc = deepSleep.GoToDeepSleep(30, "sec"); // go to deep sleep and sleep for 30 seconds
            break;
        case 2:
            vTaskDelay(pdMS_TO_TICKS(15000)); // delay 15 seconds
            rc = deepSleep.GoToDeepSleep(1, "min"); // go to deep sleep and sleep for 1 minute
            break;
        case 3:
            vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
            rc = deepSleep.GoToDeepSleep(2, "min"); // go to deep sleep and sleep for 10 minutes
            break;
    }

    ESP_LOGI(tag, "GotoDeepSleep rc=%s", rc ? "true" : "false");
}
