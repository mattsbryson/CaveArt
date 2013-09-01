#ifndef CANVAS_H
#define CANVAS_H

#include "Pixels.h"

template< template<unsigned int bit_depth, PIXEL_FORMAT> class PixelType > struct Layer {
    using _rgb8 = PixelType<8, PIXEL_FORMAT::RGB>;
    using _rgb16 = PixelType<16, PIXEL_FORMAT::RGB>;
    using _rgba16 = PixelType<16, PIXEL_FORMAT::RGBA>;
    using _rgb24 = PixelType<24, PIXEL_FORMAT::RGB>;
    using _rgb32 = PixelType<32, PIXEL_FORMAT::RGB>;
    using _rgba32 = PixelType<32, PIXEL_FORMAT::RGBA>;


    Layer(unsigned int width, unsigned int height, unsigned int init_color = 0xFFFFFFFF);
    Layer();
    ~Layer();
    Layer(const Layer& layer);

    pixel pixl_at(unsigned int x, unsigned int y);
    void modify_pixel(unsigned int x, unsigned int y, bool add, pixel pix);
    void replace_pixel(unsigned int x, unsigned int y, pixel pix);
    void convert_pixels(unsigned int bit_depth, PIXEL_FORMAT fmt) {
        int x = 0;
        int y = 0;
        while(pixels[y]) {
            while(pixels[y][x]) {
                pixels[y][x].convert_to(bit_depth, fmt);
                x++;
            }
            x = 0;
            y++;
        }
    }
};


#endif // CANVAS_H_INCLUDED
