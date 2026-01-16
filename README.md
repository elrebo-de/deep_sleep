# Deep sleep component

This repository contains an ESP-IDF component for the deep sleep function. It runs on
any ESP32 processor and is built using the ESP-IDF build system in version 5.5+.

The component is implemented as C++ class `DeepSleep`.

## Connecting the component

The constructor of class `DeepSleep` has one parameters:

| Parameter | Type of Parameter | Usage                                        |
|:----------|:------------------|:---------------------------------------------|
| tag       | std::string       | the tag to be used in the ESP log            |
| bootCount | int *             | pointer to the bootCount in RTC_DATA storage |

# Usage

The 
## API
The API of the component is located in the include directory ```include/deep_sleep.hpp``` and defines the
C++ class ```DeepSleep```

```C++
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

private:
    std::string tag = "DeepSleep";

    RTC_DATA_ATTR int bootCount = 0;
    unsigned long startTime;
};
```

# License
This component is provided under the Apache 2.0 license.
