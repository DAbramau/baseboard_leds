#pragma once

#include "config.h"

#include "internal/RgbColor.h"

class Effect
{
public:
    Effect(void);
    virtual ~Effect();
    virtual void reset(void)= 0;
    virtual uint32_t step(void) = 0;
    RgbColor getColor(uint32_t idx);

protected:
    RgbColor _color_buffer[CONFIG_MAX_EFFECT_PIXEL_NUM];
};