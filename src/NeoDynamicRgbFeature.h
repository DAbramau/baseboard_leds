#pragma once

#include "NeoPixelBus.h"

class NeoDynamicRgbFeature : public Neo3ByteElementsNoSettings
{
public:
    enum RGBOrder {
        RGB_ORDER_RGB,
        RGB_ORDER_RBG,
        RGB_ORDER_GRB,
        RGB_ORDER_GBR,
        RGB_ORDER_BRG,
        RGB_ORDER_BGR,
    };

private:
    static enum RGBOrder _rgb_order;

public:
    static void setRgbOrder(enum RGBOrder rgb_order)
    {
        _rgb_order = rgb_order;
    }

    static void applyPixelColor(uint8_t* pPixels, uint16_t indexPixel, ColorObject color)
    {
        uint8_t* p = getPixelAddress(pPixels, indexPixel);

        switch (_rgb_order)
        {
            case RGBOrder::RGB_ORDER_RGB: {
                *p++ = color.R;
                *p++ = color.G;
                *p = color.B;
            } break;

            case RGBOrder::RGB_ORDER_RBG: {
                *p++ = color.R;
                *p++ = color.B;
                *p = color.G;
            } break;

            case RGBOrder::RGB_ORDER_GRB: {
                *p++ = color.G;
                *p++ = color.R;
                *p = color.B;
            } break;

            case RGBOrder::RGB_ORDER_GBR: {
                *p++ = color.G;
                *p++ = color.B;
                *p = color.R;
            } break;

            case RGBOrder::RGB_ORDER_BRG: {
                *p++ = color.B;
                *p++ = color.R;
                *p = color.G;
            } break;

            case RGBOrder::RGB_ORDER_BGR: {
                *p++ = color.B;
                *p++ = color.G;
                *p = color.R;
            } break;
        }
    }

    static ColorObject retrievePixelColor(const uint8_t* pPixels, uint16_t indexPixel)
    {
        ColorObject color;
        const uint8_t* p = getPixelAddress(pPixels, indexPixel);

        switch (_rgb_order)
        {
            case RGBOrder::RGB_ORDER_RGB: {
                color.R = *p++;
                color.G = *p++;
                color.B = *p;
            } break;

            case RGBOrder::RGB_ORDER_RBG: {
                color.R = *p++;
                color.B = *p++;
                color.G = *p;
            } break;

            case RGBOrder::RGB_ORDER_GRB: {
                color.G = *p++;
                color.R = *p++;
                color.B = *p;
            } break;

            case RGBOrder::RGB_ORDER_GBR: {
                color.G = *p++;
                color.B = *p++;
                color.R = *p;
            } break;

            case RGBOrder::RGB_ORDER_BRG: {
                color.B = *p++;
                color.R = *p++;
                color.G = *p;
            } break;

            case RGBOrder::RGB_ORDER_BGR: {
                color.B = *p++;
                color.G = *p++;
                color.R = *p;
            } break;
        }

        return color;
    }
    
    static ColorObject retrievePixelColor_P(PGM_VOID_P pPixels, uint16_t indexPixel)
    {
        ColorObject color;
        const uint8_t* p = getPixelAddress(reinterpret_cast<const uint8_t*>(pPixels), indexPixel);
        switch (_rgb_order)
        {
            case RGBOrder::RGB_ORDER_RGB: {
                color.R = pgm_read_byte(p++);
                color.G = pgm_read_byte(p++);
                color.B = pgm_read_byte(p);
            } break;

            case RGBOrder::RGB_ORDER_RBG: {
                color.R = pgm_read_byte(p++);
                color.B = pgm_read_byte(p++);
                color.G = pgm_read_byte(p);
            } break;

            case RGBOrder::RGB_ORDER_GRB: {
                color.G = pgm_read_byte(p++);
                color.R = pgm_read_byte(p++);
                color.B = pgm_read_byte(p);
            } break;

            case RGBOrder::RGB_ORDER_GBR: {
                color.G = pgm_read_byte(p++);
                color.B = pgm_read_byte(p++);
                color.R = pgm_read_byte(p);
            } break;

            case RGBOrder::RGB_ORDER_BRG: {
                color.B = pgm_read_byte(p++);
                color.R = pgm_read_byte(p++);
                color.G = pgm_read_byte(p);
            } break;

            case RGBOrder::RGB_ORDER_BGR: {
                color.B = pgm_read_byte(p++);
                color.G = pgm_read_byte(p++);
                color.R = pgm_read_byte(p);
            } break;
        }

        return color;
    }
    
};
