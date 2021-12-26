#include "effect.h"

Effect::Effect()
{
    for (uint32_t i = 0; i < CONFIG_MAX_EFFECT_PIXEL_NUM; ++i)
    {
        _color_buffer[i] = RgbColor(0);
    }
}

Effect::~Effect()
{

}

RgbColor Effect::getColor(uint32_t idx)
{
    return _color_buffer[idx];
}
