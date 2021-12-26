#include "EffectRainbow.h"
#include "internal/Rgb48Color.h"
#include "internal/HslColor.h"

EffectRainbow::EffectRainbow(uint32_t step_period, uint32_t length) :
    _step_period(step_period),
    _length(length),
    _step(0)
{
   // reset();
}

void EffectRainbow::reset(void)
{
    for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    {
        step();
    }
}

uint32_t EffectRainbow::step(void)
{
    for (uint32_t i = CONFIG_MAX_EFFECT_PIXEL_NUM - 1; i > 0; --i)
    {
        _color_buffer[i] = _color_buffer[i - 1];
    }
    _color_buffer[0] = HslColor((float)_step / (float)_length, 1, 0.3);

    _step = (_step + 1) % _length;

    return _step_period;
}
