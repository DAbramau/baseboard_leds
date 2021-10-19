#pragma once

#include "credentials.h"
#include "version.h"

/* -------------------------------------------------------------------------- */

#ifndef BUILD_SERVER
#define BUILD_SERVER                ("unknown")
#endif

#ifndef BUILD_COMMIT_SHA
#define BUILD_COMMIT_SHA            ("undefined")
#endif

/* ---------------------------------- board --------------------------------- */

#define CONFIG_BUTTON_PIN           (0)
#define CONFIG_LED_PIN              (32)
#define CONFIG_TST_PIN              (19)
#define CONFIG_NEOPIXEL_CH0_PIN     (21)
#define CONFIG_NEOPIXEL_CH1_PIN     (22)
#define CONFIG_NEOPIXEL_CH2_PIN     (23)
#define CONFIG_NEOPIXEL_CH3_PIN     (25)
#define CONFIG_NEOPIXEL_CH4_PIN     (26)
#define CONFIG_NEOPIXEL_CH5_PIN     (27)


/* ------------------------------- led config ------------------------------- */

#define CONFIG_NEOPIXEL_CH0_NUM     (12)
#define CONFIG_NEOPIXEL_CH1_NUM     (12)
#define CONFIG_NEOPIXEL_CH2_NUM     (12)
#define CONFIG_NEOPIXEL_CH3_NUM     (12)
#define CONFIG_NEOPIXEL_CH4_NUM     (12)
#define CONFIG_NEOPIXEL_CH5_NUM     (12)

/* ------------------------------- wifi config ------------------------------ */

#define CONFIG_MDNS_NAME            ("bbled")

/* ---------------------------------- debug --------------------------------- */

#define CONFIG_UART_BAUD_RATE       (921600)
