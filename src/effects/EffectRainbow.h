#pragma once

#include "Effect.h"

class EffectRainbow: public Effect
{
public:
    EffectRainbow(uint32_t step_period, uint32_t length);
    void reset(void);
    uint32_t step(void);

private:
    const uint32_t _step_period;
    const uint32_t _length;

    uint32_t _step;
};