/*
 * deep_sleep.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef DEEP_SLEEP_HPP_
#define DEEP_SLEEP_HPP_

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_sleep.h"
#include "esp_log.h"

/* class DeepSleep
   wrapper for esp deep sleep functionality
*/
class DeepSleep {
public:
    // Constructor
	DeepSleep( std::string tag,
	           int *bootCount
	         );
	virtual ~DeepSleep();

	bool GoToDeepSleep( unsigned long sleepTime,
                        std::string sleepTimeUnit // {"min", "sec", "milliSec", "microSec"}
	);

private:
    std::string tag = "DeepSleep";
    esp_sleep_wakeup_cause_t wakeup_reason; // the wakeup reason for the last boot
};

#endif /* DEEP_SLEEP_HPP_ */
