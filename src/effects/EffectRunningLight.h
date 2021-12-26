#pragma once

#include "Effect.h"

class EffectRunningLight: public Effect
{
public:
    EffectRunningLight(uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size, uint32_t light_num, uint32_t period);
    void reset(void);
    uint32_t step(void);

private:
    const uint32_t _step_period;
    const RgbColor* _color_list;
    const uint32_t _color_list_size;
    const uint32_t _light_num;
    const uint32_t _period;

    uint32_t _step;
};