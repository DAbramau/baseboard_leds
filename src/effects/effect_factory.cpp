#include "effect_factory.h"

#include "utils.h"

#include "EffectSequence.h"
#include "EffectRandom.h"
#include "EffectRunningLight.h"
#include "EffectRainbow.h"

static RgbColor _color_list[] = {
    RgbColor(150, 0, 0),
    RgbColor(0, 150, 0),
    RgbColor(0, 0, 150),
    RgbColor(100, 0, 100),
    RgbColor(100, 100, 0),
    RgbColor(0, 100, 100)
};

uint32_t get_number_of_effects(void)
{
    return 4;
}

Effect* get_effect(uint32_t effect_index)
{
    switch (effect_index)
    {
    case 0:
        return new EffectRainbow(70, 150);
    
    case 1:
        return new EffectRandom(400, _color_list, ARRAY_SIZE(_color_list));

    case 2:
        return new EffectRunningLight(30, _color_list, ARRAY_SIZE(_color_list), 2, 7);

    case 3:
        return new EffectSequence(400, 7, _color_list, ARRAY_SIZE(_color_list));
    
    default:
        return nullptr;
    }
}
