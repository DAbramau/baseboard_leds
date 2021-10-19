#include "config.h"

#include <Arduino.h>

#include "NeoPixelBus.h"

#include "utils.h"

/* -------------------------------------------------------------------------- */

static NeoPixelBus<NeoGrbFeature, NeoWs2813Method> strip_0(CONFIG_NEOPIXEL_CH0_NUM, CONFIG_NEOPIXEL_CH0_PIN);

static RgbColor _color_list[] = 
{
    RgbColor(127, 0,0),
    RgbColor(0, 127, 0),
    RgbColor(0, 0, 127)
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
        strip_0.SetPixelColor(i, i < frame_index ? _color_list[color_index] : 0);
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
        strip_0.SetPixelColor(i, _color_list[rand() % ARRAY_SIZE(_color_list)]);
    }
}

/* -------------------------------------------------------------------------- */

static void _mode_3(void)
{
    static uint32_t step = 0;
    static RgbColor color = 0;

    static RgbColor buffer[CONFIG_NEOPIXEL_CH0_NUM];

    for (uint32_t i = CONFIG_NEOPIXEL_CH0_NUM - 1; i > 0; --i)
    {
        buffer[i] = buffer[i - 1];
    }

    if (step == 0)
    {
        color = RgbColor(rand() % 256, rand() % 256 , rand() % 256);
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
        strip_0.SetPixelColor(i, buffer[i]);
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
        strip_0.Show();

        delay(100);
    }
}

/* -------------------------------------------------------------------------- */

void neo_led_init(void)
{
    strip_0.Begin();
    strip_0.Show();

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
