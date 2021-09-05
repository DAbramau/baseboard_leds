#pragma once

#include "credentials.h"
#include "version.h"

#ifndef BUILD_SERVER
#define BUILD_SERVER                ("unknown")
#endif

#ifndef BUILD_COMMIT_SHA
#define BUILD_COMMIT_SHA            ("undefined")
#endif

#define CONFIG_UART_BAUD_RATE       (921600)

#define CONFIG_BUTTON_PIN           (0)

#define CONFIG_NEOPIXEL_CH0_PIN     (26)
#define CONFIG_NEOPIXEL_CH0_NUM     (5)

#define CONFIG_MDNS_NAME            ("bbled")
