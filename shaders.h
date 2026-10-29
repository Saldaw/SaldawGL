#pragma once
#include "our_gl.h"
#include "model.h"

struct PhongShader : public IShader {
    Matrix mv, proj, vp;
    Vec3f light;
    Vec3f eye;
    Model* model;

    Vec3f vn[3];
    Vec2f vu[3];
    Vec4f vt[3];

    PhongShader(Model* m, Matrix a, Matrix b, Matrix c, Vec3f l, Vec3f e) {
        model = m;
        mv = a;
        proj = b;
        vp = c;
        light = l;
        eye = e;
    }

    Vec4f vertex(int i, int j) {
        Vec3f v = model->vert(i, j);
        Vec3f n = model->normal(i, j);
        Vec4f clip = proj * mv * Vec4f(v.x, v.y, v.z, 1.0f);
        Vec4f cn = mv * Vec4f(n.x, n.y, n.z, 0.0f);
        vn[j] = Vec3f(cn.x, cn.y, cn.z).normalized();
        vu[j] = model->uv(i, j);
        vt[j] = clip;
        return clip;
    }

    bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f n = (vn[0] * bar.x + vn[1] * bar.y + vn[2] * bar.z).normalized();
        Vec2f uv = vu[0] * bar.x + vu[1] * bar.y + vu[2] * bar.z;

        TGAColor tc = model->diffuse(uv);
        Vec3f mc(tc.bgra[2] / 255.0f, tc.bgra[1] / 255.0f, tc.bgra[0] / 255.0f);

        Vec4f ndc = vt[0] * bar.x + vt[1] * bar.y + vt[2] * bar.z;
        Vec3f fp(ndc.x / ndc.w, ndc.y / ndc.w, ndc.z / ndc.w);

        Vec3f l = light.normalized();
        Vec3f v = (eye - fp).normalized();
        Vec3f r = (n * (n * l) * 2.0f - l).normalized();

        float amb = 0.2f;
        float dif = std::max(0.0f, n * l) * 0.8f;
        float spec = std::pow(std::max(0.0f, r * v), 32.0f) * 0.5f;

        float I = amb + dif + spec;
        if (I > 1.0f) I = 1.0f;
        if (I < 0.0f) I = 0.0f;

        color = TGAColor(
            (unsigned char)(mc.x * 255 * I),
            (unsigned char)(mc.y * 255 * I),
            (unsigned char)(mc.z * 255 * I),
            255
        );
        return false;
    }
};