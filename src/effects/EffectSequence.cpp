#include "EffectSequence.h"

EffectSequence::EffectSequence(
    uint32_t led_num, uint32_t step_period, const RgbColor* color_list, uint32_t color_list_size) :
    _led_num(led_num),
    _step_period(step_period),
    _color_list(color_list),
    _color_list_size(color_list_size),
    _color_index(0),
    _frame_index(0)
{
    reset();
}

void EffectSequence::reset(void)
{
    for (uint32_t i = 0; i < _led_num; ++i)
    {
        _color_buffer[i] = RgbColor(0);
    }
}

uint32_t EffectSequence::step(void)
{
    if (_frame_index == (_led_num + 1))
    {
        for (uint32_t i = 0; i < _led_num; ++i)
        {
            _color_buffer[i] = RgbColor(0);
            _color_index = (_color_index + 1) % _color_list_size;
        }
        _frame_index = 1;
    }
    else
    {
        if (_frame_index > 0)
        {
            _color_buffer[_frame_index - 1] = _color_list[_color_index];
        }
        ++_frame_index;
    }

    return _step_period;
}
