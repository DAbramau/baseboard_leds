#include "config.h"

#include <Arduino.h>

#include "Adafruit_NeoPixel.h"

#include "utils.h"

/* -------------------------------------------------------------------------- */

static Adafruit_NeoPixel strip_0(CONFIG_NEOPIXEL_CH0_NUM, CONFIG_NEOPIXEL_CH0_PIN, NEO_GRB + NEO_KHZ800);

static uint32_t _color_list[] = 
{
    strip_0.Color(127, 127, 127),
    strip_0.Color(127, 0, 0),
    strip_0.Color(0, 127, 0),
    strip_0.Color(0, 0, 127)
};

static uint32_t _mode_index = 0;

/* -------------------------------------------------------------------------- */

#define TASK_NEO_LED_STACK_SIZE             (3000)
#define TASK_NEO_LED_PRIORITY               (configMAX_PRIORITIES - 5)  

static TaskHandle_t _neo_led_task_handler;

/* -------------------------------------------------------------------------- */

static void _mode_1(void)
{
    static uint32_t step = 0;

    const uint32_t color_index = step / (CONFIG_NEOPIXEL_CH0_NUM + 1);
    const uint32_t frame_index = step % (CONFIG_NEOPIXEL_CH0_NUM + 1);

    for (uint32_t i = 0; i < CONFIG_NEOPIXEL_CH0_NUM; ++i)
    {
        strip_0.setPixelColor(i, i < frame_index ? _color_list[color_index] : 0);
    }

    ++step;
    if (step == (CONFIG_NEOPIXEL_CH0_NUM + 1) * ARRAY_SIZE(_color_list))
    {
        step = 0;
    }
}

/* -------------------------------------------------------------------------- */

static void _mode_2(void)
{
    for (uint32_t i = 0; i < CONFIG_NEOPIXEL_CH0_NUM; ++i)
    {
        strip_0.setPixelColor(i, _color_list[rand() % ARRAY_SIZE(_color_list)]);
    }
}

/* -------------------------------------------------------------------------- */

static void _mode_3(void)
{
    static uint32_t step = 0;
    static uint32_t color = 0;

    static uint32_t buffer[CONFIG_NEOPIXEL_CH0_NUM] = {0};

    for (uint32_t i = CONFIG_NEOPIXEL_CH0_NUM - 1; i > 0; --i)
    {
        buffer[i] = buffer[i - 1];
    }

    if (step == 0)
    {
        color = strip_0.Color(rand() % 256, rand() % 256 , rand() % 256);
    }

    if (step < 2)
    {
        buffer[0] = color;
    }
    else
    {
        buffer[0] = 0;
    }

    for (uint32_t i = 0; i < CONFIG_NEOPIXEL_CH0_NUM; ++i)
    {
        strip_0.setPixelColor(i, buffer[i]);
    }

    ++step;
    if (step == 6)
    {
        step = 0;
    }
}

/* -------------------------------------------------------------------------- */

static void(*_modes[])(void) = 
{
    _mode_1,
    _mode_2,
    _mode_3
};

/* -------------------------------------------------------------------------- */

static void neo_led_task(void* parameter)
{
    for (;;)
    {
        _modes[_mode_index]();
        strip_0.show();

        delay(100);
    }
}

/* -------------------------------------------------------------------------- */

void neo_led_init(void)
{
    strip_0.begin();
    strip_0.show();
    strip_0.setBrightness(255);

    xTaskCreate(neo_led_task,
                "neo_led",
                TASK_NEO_LED_STACK_SIZE,
                NULL,
                TASK_NEO_LED_PRIORITY,
                &_neo_led_task_handler);
}

/* -------------------------------------------------------------------------- */

void neo_led_next_mode(void)
{
    _mode_index = (_mode_index + 1) % ARRAY_SIZE(_modes);
}

/* -------------------------------------------------------------------------- */
