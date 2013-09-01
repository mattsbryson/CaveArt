#define RED_3_3_2 0xE0      //use with & char
#define GRN_3_3_2 0x1C
#define BLU_3_3_2 0x03

#define RED_5_6_5 0xF800    //use with & short
#define GRN_5_6_5 0x07E0
#define BLU_5_6_5 0x001F

#define RED_4_4_4_4 0xF000  //use with & short
#define GRN_4_4_4_4 0x0F00
#define BLU_4_4_4_4 0x00F0
#define ALP_4_4_4_4 0x000F

#define RED_8_8_8_8 0xFF000000  //use with & long / & int, 0xFFFFFFFF
#define GRN_8_8_8_8 0x00FF0000
#define BLU_8_8_8_8 0x0000FF00
#define ALP_8_8_8_8 0x000000FF

#define RED_8_8_8 RED_8_8_8_8
#define GRN_8_8_8 GRN_8_8_8_8
#define BLU_8_8_8 BLU_8_8_8_8

#define RED_10_10_10 0xFFC00000 //use with & long / & int, 0x00000000
#define GRN_10_10_10 0x003FF000
#define BLU_10_10_10 0x00000FFC

#define RED_16_16_16_16 0xFFFF000000000000  //use with & long long
#define GRN_16_16_16_16 0x0000FFFF00000000
#define BLU_16_16_16_16 0x00000000FFFF0000
#define ALP_16_16_16_16 0x000000000000FFFF

enum class PIXEL_FORMAT : char {
    PALETTE,
    RGB,
    RGBA
};

template<unsigned bit_depth, PIXEL_FORMAT fmt> struct Pixel {};

template<> struct Pixel<8, PIXEL_FORMAT::RGB> {
    unsigned char color;
    Pixel(unsigned char red, unsigned char green, unsigned char blue) {
        color = 0xFF & (((red << 5) & RED_3_3_2) & ((green << 2) & GRN_3_3_2) & (blue & BLU_3_3_2));
    }
    Pixel(unsigned char clr) { color = clr; }

    /*
    template<unsigned int bit_depth, PIXEL_FORMAT fmt> Pixel<bit_depth, fmt> convert_to() {
        char red, grn, blu;
        red = ((color & RED_3_3_2) >> 5);
        grn = ((color & GRN_3_3_2) >> 2);
        blu = (color & BLU_3_3_2);
        if(fmt == PIXEL_FORMAT::RGB) {
            int MAX_VAL = (((powl(2,bit_depth)) - 1) / 3);
        } else if(fmt == PIXEL_FORMAT::RGBA) {
            int MAX_VAL = (((powl(2,bit_depth)) - 1) / 4);
        }
    }
    */

    Pixel<16, PIXEL_FORMAT::RGB> convert_to_rgb16() {
        unsigned char red, grn, blu;
        red = ((color & RED_3_3_2) >> 5);
        grn = ((color & GRN_3_3_2) >> 2);
        blu = (color & BLU_3_3_2);

        red = ((red/7) * 31);   //5
        grn = ((grn/7) * 63);   //6
        blu = ((blu/3) * 31);   //5

        Pixel<16, PIXEL_FORMAT::RGB> _16rgb(red, grn, blu);
        return _16rgb;
    }
};

template<> struct Pixel<16, PIXEL_FORMAT::RGB> {
    unsigned short color;
    Pixel(unsigned char red, unsigned char green, unsigned char blue) {
        color = (0xFFFF & (((red << 11) & RED_5_6_5) & ((green << 5) & GRN_5_6_5) & (blue & BLU_5_6_5)));
    }
    Pixel(short clr) { color = clr; }
};

template<> struct Pixel<16, PIXEL_FORMAT::RGBA> {
    unsigned short color;
    Pixel(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0) {
        color = (0xFFFF & (((red << 12) & RED_4_4_4_4) & ((green << 8) & GRN_4_4_4_4) & ((blue << 4) & BLU_4_4_4_4) & (alpha & ALP_4_4_4_4)));
    }
};

template<> struct Pixel<24, PIXEL_FORMAT::RGB> {	//WOO TRUE COLOR
    unsigned char red;
    unsigned char green;
    unsigned char blue;

	Pixel(unsigned char r, unsigned char g, unsigned char b) {
		red = r;
		green = g;
		blue = b;
	}

	Pixel(unsigned int color) {
		red = (color & RED_8_8_8);
		green = (color & GRN_8_8_8);
		blue = (color & BLU_8_8_8);
	}
};

template<> struct Pixel<32, PIXEL_FORMAT::RGBA> {   //True color... BUT WITH TRANPARENCY
    unsigned long color;

    Pixel() { color = 0x00000000; }

    Pixel(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0) {
        color = (0xFFFFFFFF & (((red << 24) & RED_8_8_8_8) & ((green << 16) & GRN_8_8_8_8) & ((blue << 8) & BLU_8_8_8_8) & (alpha & ALP_8_8_8_8)));
    }

    Pixel(unsigned long clr) {
        color = clr;
    }

    Pixel(unsigned short red_green, unsigned short blue_alpha) {
        color = (0xFFFFFFFF & ((red_green << 16) & (RED_8_8_8_8 | GRN_8_8_8_8)) & ((blue_alpha) & (BLU_8_8_8_8 | ALP_8_8_8_8)));
    }
};

struct pixel_palette {
    typedef Pixel<32, PIXEL_FORMAT::RGBA> _32rgba_pixl;
    _32rgba_pixl* pixels;
    pixel_palette(unsigned char red = 0, unsigned char grn = 0, unsigned char blu = 0, unsigned char alp = 0, unsigned char incr = 4, unsigned int pal_size = 256) {

        pixels = new _32rgba_pixl[pal_size];
        unsigned int i = 0;

        while(i < pal_size) {
            pixels[i] = _32rgba_pixl(red, grn, blu, alp);
            if((red <= grn) && (red <= blu) && (red <= alp)) {
                red += incr;
            } else if((grn < red) && (grn <= blu) && (grn <= alp)) {
                grn += incr;
            } else if((blu < red) && (blu < grn) && (blu <= alp)) {
                blu += incr;
            } else {
                alp += incr;
            }
            i++;
        }
    }

    ~pixel_palette() { delete pixels; }
};



template<> struct Pixel<8, PIXEL_FORMAT::PALETTE> {
    static pixel_palette _8_bit_pal();
    unsigned char index;
    Pixel(char indx) {
        index = indx;
    }

};
