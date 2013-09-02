#ifndef CANVAS_H
#define CANVAS_H

#include "Pixels.h"

template< typename PixelType > struct Layer {

    PixelType** pixels;

    Layer(unsigned int width, unsigned int height, unsigned int init_color = 0xFFFFFFFF);
    Layer();
    ~Layer();
    Layer(const Layer& layer);

    PixelType pixel_at(unsigned int x, unsigned int y);
    void modify_pixel(unsigned int x, unsigned int y, bool add, PixelType pix);
    void replace_pixel(unsigned int x, unsigned int y, PixelType pix);
    template<unsigned int bit_depth, PIXEL_FORMAT fmt> void convert_pixels();
};


#endif // CANVAS_H_INCLUDED
