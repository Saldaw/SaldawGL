#pragma once
#include "geometry.h"
#include "tgaimage.h"

struct IShader {
    virtual ~IShader() {}
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void triangle(Vec4f* clip_v, IShader& shader, TGAImage& image, float* zbuffer, Matrix Viewport, int width, int height);