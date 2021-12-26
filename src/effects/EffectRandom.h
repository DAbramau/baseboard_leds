#pragma once

#include "Effect.h"

class EffectRandom: public Effect
{
public:
    EffectRandom(uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size);
    void reset(void);
    uint32_t step(void);

private:
    const uint32_t _step_period;
    const RgbColor* _color_list;
    const uint32_t _color_list_size;
};