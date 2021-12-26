#include "EffectRunningLight.h"

EffectRunningLight::EffectRunningLight(
    uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size, uint32_t light_num, uint32_t period) :
    _step_period(step_period),
    _color_list(color_list),
    _color_list_size(color_list_size),
    _light_num(light_num),
    _period(period),
    _step(0)
{
    reset();
}

void EffectRunningLight::reset(void)
{
    for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    {
        _color_buffer[i] = RgbColor(0);
    }
}

uint32_t EffectRunningLight::step(void)
{
    static bool red = true;
    for (uint32_t i = CONFIG_MAX_EFFECT_PIXEL_NUM - 1; i > 0; --i)
    {
        _color_buffer[i] = _color_buffer[i - 1];
    }
    if (_step == 0) 
    {
        _color_buffer[0] = red ? _color_list[0] : _color_list[1];
        red = !red;
    }
    else if (_step < _light_num)
    {
        _color_buffer[0] = _color_buffer[1];
    }
    else
    {
        _color_buffer[0] = RgbColor(0);
    }

    _step = (_step + 1) % _period;

    return _step_period;
}
