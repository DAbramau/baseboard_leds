#include "config.h"

#include <Arduino.h>

#include "NeoPixelBus.h"

#include "NeoDynamicRgbFeature.h"

#include "utils.h"

/* -------------------------------------------------------------------------- */

#define TASK_NEO_LED_STACK_SIZE             (3000)
#define TASK_NEO_LED_PRIORITY               (configMAX_PRIORITIES - 5)  

/* -------------------------------------------------------------------------- */

typedef NeoPixelBus<NeoDynamicRgbFeature, NeoEsp32BitBangWs2813Method> stripe_t;

typedef struct {
    uint32_t pin;
    uint32_t led_number;
    NeoDynamicRgbFeature::RGBOrder rgb_order;
} stripe_config_t;

typedef struct {
    uint32_t step;
    stripe_t* stripe;
} stripe_state_t;

/* -------------------------------------------------------------------------- */

static stripe_config_t _stripe_config_list[2] = {
    {CONFIG_NEOPIXEL_CH0_PIN, CONFIG_NEOPIXEL_CH0_NUM, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_GRB},
    {CONFIG_NEOPIXEL_CH5_PIN, CONFIG_NEOPIXEL_CH5_NUM, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB}
};

static stripe_state_t _stripe_state_list[2] = { 0 };

static stripe_config_t* _stripe_config = NULL;
static stripe_state_t* _stripe_state = NULL;

static RgbColor _color_list[] = {
    RgbColor(127, 0, 0),
    RgbColor(0, 127, 0),
    RgbColor(0, 0, 127)
};

/* -------------------------------------------------------------------------- */

static TaskHandle_t _neo_led_task_handler;

/* -------------------------------------------------------------------------- */

static void _mode_1(void)
{
    const uint32_t color_index = _stripe_state->step / (_stripe_config->led_number + 1);
    const uint32_t frame_index = _stripe_state->step % (_stripe_config->led_number + 1);

    for (uint32_t j = 0; j < _stripe_config->led_number; ++j)
    {
        _stripe_state->stripe->SetPixelColor(j, j < frame_index ? _color_list[color_index] : 0);
    }

    INCREMENT_WRAP(_stripe_state->step, (_stripe_config->led_number + 1) * ARRAY_SIZE(_color_list));
}

/* -------------------------------------------------------------------------- */

static void _mode_2(void)
{
    for (uint32_t j = 0; j < _stripe_config->led_number; ++j)
    {
        _stripe_state->stripe->SetPixelColor(j, _color_list[rand() % ARRAY_SIZE(_color_list)]);
    }
}

/* -------------------------------------------------------------------------- */

static void _mode_3(void)
{
    for (uint32_t j = _stripe_config->led_number - 1; j > 0; --j)
    {
        _stripe_state->stripe->SetPixelColor(j, _stripe_state->stripe->GetPixelColor(j - 1));
    }
    if (_stripe_state->step == 0)
    {
        _stripe_state->stripe->SetPixelColor(0, HslColor((float)(rand() % 256) / (float)(256), 1, 0.5));
    } else if (_stripe_state->step < 2)
    {
        _stripe_state->stripe->SetPixelColor(0, _stripe_state->stripe->GetPixelColor(1));
    } else
    {
        _stripe_state->stripe->SetPixelColor(0, 0);
    }

    INCREMENT_WRAP(_stripe_state->step, 20);
}

/* -------------------------------------------------------------------------- */

static void _mode_4(void)
{
    const uint32_t length = 150;

    for (uint32_t j = 0; j < _stripe_config->led_number; ++j)
    {
        _stripe_state->stripe->SetPixelColor(_stripe_config->led_number - j - 1, HslColor((float)((j + _stripe_state->step) % (length + 1))/(float)(length), 1, 0.3));
    }

    INCREMENT_WRAP(_stripe_state->step, length);
}

/* -------------------------------------------------------------------------- */

static void(*_modes[])(void) = 
{
    _mode_1,
    _mode_2,
    _mode_3,
    _mode_4
};

static void(*_mode)(void) = _mode_1;
static uint32_t _mode_index = 0;
static uint32_t _pending_mode = 0;

/* -------------------------------------------------------------------------- */

static void neo_led_task(void* parameter)
{
    delay(2000);
    for (;;)
    {
        if (_pending_mode != _mode_index)
        {
            _mode_index = _pending_mode;
            _mode = _modes[_mode_index];
            for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
            {
                _stripe_state_list[i].step = 0;
            }
        }

        for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
        {
            _stripe_state = &_stripe_state_list[i];
            _stripe_config = &_stripe_config_list[i];
            NeoDynamicRgbFeature::setRgbOrder(_stripe_config->rgb_order);
            _mode();
        }
        for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
        {
            _stripe_state_list[i].stripe->Show();
        }

        delay(70);
    }
}

/* -------------------------------------------------------------------------- */

void neo_led_init(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
    {
        _stripe_state_list[i].stripe = new stripe_t(_stripe_config_list[i].led_number, _stripe_config_list[i].pin);
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
    {
        _stripe_state_list[i].stripe->Begin();
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_state_list); ++i)
    {
        _stripe_state_list[i].stripe->Show();
    }

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
    _pending_mode = (_mode_index + 1) % ARRAY_SIZE(_modes);
}

/* -------------------------------------------------------------------------- */

void neo_led_set_mode(uint32_t mode)
{
    if (mode < ARRAY_SIZE(_modes))
    {
        _pending_mode = mode;
    }
}

/* -------------------------------------------------------------------------- */
