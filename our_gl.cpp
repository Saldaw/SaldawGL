#include "our_gl.h"
#include <algorithm>
#include <cmath>

void triangle(Vec4f* clip, IShader& shader, TGAImage& image, float* zbuf, Matrix vp, int W, int H) {
    Vec2f p[3];
    float z[3];

    for (int i = 0; i < 3; i++) {
        if (clip[i].w <= 0) return;
        Vec4f n = clip[i] * (1.0f / clip[i].w);
        Vec4f s = vp * Vec4f(n.x, n.y, n.z, 1.0f);
        p[i] = Vec2f(s.x, s.y);
        z[i] = n.z;
    }

    float xmin = p[0].x, xmax = p[0].x, ymin = p[0].y, ymax = p[0].y;
    for (int i = 1; i < 3; i++) {
        xmin = std::min(xmin, p[i].x);
        xmax = std::max(xmax, p[i].x);
        ymin = std::min(ymin, p[i].y);
        ymax = std::max(ymax, p[i].y);
    }

    int x0 = std::max(0, (int)std::floor(xmin));
    int x1 = std::min(W - 1, (int)std::ceil(xmax));
    int y0 = std::max(0, (int)std::floor(ymin));
    int y1 = std::min(H - 1, (int)std::ceil(ymax));

    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            Vec3f bc = barycentric(p[0], p[1], p[2], Vec2f((float)x, (float)y));
            if (bc.x < -0.01f || bc.y < -0.01f || bc.z < -0.01f) continue;

            Vec3f bcc(bc.x / clip[0].w, bc.y / clip[1].w, bc.z / clip[2].w);
            bcc = bcc * (1.0f / (bcc.x + bcc.y + bcc.z));

            float d = bcc.x * z[0] + bcc.y * z[1] + bcc.z * z[2];
            int id = x + y * W;

            if (d < zbuf[id]) {
                TGAColor c;
                if (!shader.fragment(bcc, c)) {
                    zbuf[id] = d;
                    image.set(x, y, c);
                }
            }
        }
    }
}