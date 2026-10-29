#pragma once
#include <fstream>
#include <vector>
#include <iostream>

#pragma pack(push,1)
struct TGAHeader {
    char idlength;
    char colormaptype;
    char datatypecode;
    short colormaporigin;
    short colormaplength;
    char colormapdepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitsperpixel;
    char imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {
    unsigned char bgra[4];
    TGAColor() : bgra{ 0,0,0,0 } {}
    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : bgra{ B, G, R, A } {}
};

class TGAImage {
    int width, height, bytespp;
    std::vector<unsigned char> data;
public:
    enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };
    TGAImage() : width(0), height(0), bytespp(0) {}
    TGAImage(int w, int h, int bpp) : width(w), height(h), bytespp(bpp) {
        data.resize(width * height * bytespp, 0);
    }
    bool read_tga_file(const char* filename);
    bool write_tga_file(const char* filename);

    TGAColor get(int x, int y) const {
        if (!data.size() || x < 0 || y < 0 || x >= width || y >= height) return TGAColor();
        TGAColor c;
        const unsigned char* p = &data[(x + y * width) * bytespp];
        c.bgra[0] = p[0];
        c.bgra[1] = p[1];
        c.bgra[2] = p[2];
        c.bgra[3] = (bytespp == 4) ? p[3] : 255;
        return c;
    }

    void set(int x, int y, TGAColor c) {
        if (!data.size() || x < 0 || y < 0 || x >= width || y >= height) return;
        unsigned char* p = &data[(x + y * width) * bytespp];
        p[0] = c.bgra[0];
        p[1] = c.bgra[1];
        p[2] = c.bgra[2];
        if (bytespp == 4) p[3] = c.bgra[3];
    }
    void flip_vertically();
    int get_width() const { return width; }
    int get_height() const { return height; }
};