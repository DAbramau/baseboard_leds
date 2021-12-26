#pragma once

#include "Effect.h"

class EffectSequence: public Effect
{
public:
    EffectSequence(uint32_t led_num, uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size);
    void reset(void);
    uint32_t step(void);

private:
    const uint32_t _led_num;
    const uint32_t _step_period;
    const RgbColor* _color_list;
    const uint32_t _color_list_size;

    uint32_t _color_index;
    uint32_t _frame_index;
};