#include "config.h"

#include <Arduino.h>

#include "NeoPixelBus.h"

#include "NeoDynamicRgbFeature.h"

#include "effects/effect_factory.h"

#include "utils.h"

/* -------------------------------------------------------------------------- */

#define TASK_NEO_LED_STACK_SIZE             (30000)
#define TASK_NEO_LED_PRIORITY               (configMAX_PRIORITIES - 3)  


static portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;

/* -------------------------------------------------------------------------- */

typedef NeoPixelBus<NeoDynamicRgbFeature, NeoEsp32BitBangWs2813Method> stripe_t;

typedef struct {
    uint32_t pin;
    uint32_t led_number;
    uint32_t offset;
    bool reverse;
    NeoDynamicRgbFeature::RGBOrder rgb_order;
    stripe_t* stripe;
} stripe_config_t;

/* -------------------------------------------------------------------------- */

static stripe_config_t _stripe_config_list[] = {
    {CONFIG_NEOPIXEL_CH0_PIN, CONFIG_NEOPIXEL_CH0_NUM, 0, false, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_GRB, nullptr},
    {CONFIG_NEOPIXEL_CH1_PIN, CONFIG_NEOPIXEL_CH1_NUM, 208, false, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr},
    // {CONFIG_NEOPIXEL_CH1_PIN, CONFIG_NEOPIXEL_CH1_NUM, 0, false, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr},
    {CONFIG_NEOPIXEL_CH2_PIN, CONFIG_NEOPIXEL_CH2_NUM, 0, true, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr}
    // {CONFIG_NEOPIXEL_CH3_PIN, CONFIG_NEOPIXEL_CH3_NUM, 0, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr},
    // {CONFIG_NEOPIXEL_CH4_PIN, CONFIG_NEOPIXEL_CH4_NUM, 0, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr},
    // {CONFIG_NEOPIXEL_CH5_PIN, CONFIG_NEOPIXEL_CH5_NUM, 0, NeoDynamicRgbFeature::RGBOrder::RGB_ORDER_RGB, nullptr}
};

/* -------------------------------------------------------------------------- */

static TaskHandle_t _neo_led_task_handler;

static uint32_t _effect_index = 0;
static uint32_t _pending_effect = 0;


/* -------------------------------------------------------------------------- */

static void neo_led_task(void* parameter)
{
    static Effect* _effect = get_effect(_effect_index);
    _effect->reset();
    for (;;)
    {
        if (_pending_effect != _effect_index)
        {
            _effect_index = _pending_effect;
           // delete _effect;
            _effect = get_effect(_effect_index);
            log_i("new effect %X", _effect);
            _effect->reset();
        }

        uint32_t pause = _effect->step();

        for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_config_list); ++i)
        {
            NeoDynamicRgbFeature::setRgbOrder(_stripe_config_list[i].rgb_order);
            if (_stripe_config_list[i].reverse)
            {
                for (uint32_t j = 0; j < _stripe_config_list[i].led_number; ++j)
                {
                    _stripe_config_list[i].stripe->SetPixelColor(_stripe_config_list[i].led_number - j - 1, _effect->getColor(j + _stripe_config_list[i].offset));
                }
            }
            else
            {
                for (uint32_t j = 0; j < _stripe_config_list[i].led_number; ++j)
                {
                    _stripe_config_list[i].stripe->SetPixelColor(j, _effect->getColor(j + _stripe_config_list[i].offset));
                }
            }
            portENTER_CRITICAL(&_mux);
            _stripe_config_list[i].stripe->Show();
            portEXIT_CRITICAL(&_mux);
        }

        delay(pause);
    }
}

/* -------------------------------------------------------------------------- */

void neo_led_init(void)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(_stripe_config_list); ++i)
    {
        _stripe_config_list[i].stripe = new stripe_t(_stripe_config_list[i].led_number, _stripe_config_list[i].pin);
        _stripe_config_list[i].stripe->Begin();
        _stripe_config_list[i].stripe->ClearTo(0);
        _stripe_config_list[i].stripe->Show();
    }

    xTaskCreatePinnedToCore(neo_led_task,
                "neo_led",
                TASK_NEO_LED_STACK_SIZE,
                NULL,
                TASK_NEO_LED_PRIORITY,
                &_neo_led_task_handler, 0);
}

/* -------------------------------------------------------------------------- */

void neo_led_next_mode(void)
{
    _pending_effect = (_effect_index + 1) % get_number_of_effects();
}

/* -------------------------------------------------------------------------- */

void neo_led_set_mode(uint32_t effect_index)
{
    // if (mode < ARRAY_SIZE(_modes))
    // {
        _pending_effect = effect_index;
    // }
}

/* -------------------------------------------------------------------------- */

void neo_led_set_config_mode(void)
{
    // _pending_mode = UINT32_MAX;
}

/* -------------------------------------------------------------------------- */

void neo_led_config_next_stripe(void)
{
    // _pending_stripe_index = (_config_stripe_index + 1) % ARRAY_SIZE(_stripe_state_list);
}

/* -------------------------------------------------------------------------- */

void neo_led_config_next_step(void)
{
    // _pending_step = (_config_step + 1) % 1024;
}

/* -------------------------------------------------------------------------- */

void neo_led_config_previous_step(void)
{
    // if (_config_step == 0)
    // {
    //     _pending_step = 1024;
    // }
    // else
    // {
    //     _pending_step = _config_step - 1;
    // }
}

/* -------------------------------------------------------------------------- */
