#include "EffectRandom.h"

EffectRandom::EffectRandom(
    uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size) :
    _step_period(step_period),
    _color_list(color_list),
    _color_list_size(color_list_size)
{
    reset();
}

void EffectRandom::reset(void)
{
    for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    {
        _color_buffer[i] = RgbColor(0);
    }
}

uint32_t EffectRandom::step(void)
{
    for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    {
        // _color_buffer[i] = (i % 20) >= 10 ? _color_list[0] : _color_list[1];
    }

    _color_buffer[0] = _color_list[1];
    _color_buffer[305] = _color_list[2];

    // for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    // {
    //     _color_buffer[i] = _color_list[rand() % _color_list_size];
    // }

    return _step_period;
}
