#include <iostream>
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
#include "our_gl.h"
#include "shaders.h"

const int W = 800;
const int H = 800;

struct Camera {
    Vec3f eye, center, up;
    Camera(Vec3f e, Vec3f c, Vec3f u) : eye(e), center(c), up(u) {}
    Matrix view() { return lookat(eye, center, up); }
};

int main() {
    Model model("african_head.obj");

    TGAImage image(W, H, TGAImage::RGB);
    float* zbuf = new float[W * H];
    for (int i = 0; i < W * H; i++) zbuf[i] = 1e9;

    Camera cam(Vec3f(1, 1, 3), Vec3f(0, 0, 0), Vec3f(0, 1, 0));

    Matrix mv = cam.view();
    Matrix proj = perspective(45.0f, (float)W / H, 0.1f, 100.0f);
    Matrix vp = viewport(W / 8, H / 8, W * 3 / 4, H * 3 / 4);

    Vec3f light(0, 0, 1);
    PhongShader shader(&model, mv, proj, vp, light, cam.eye);

    for (int i = 0; i < model.nfaces(); i++) {
        Vec4f c[3];
        for (int j = 0; j < 3; j++) c[j] = shader.vertex(i, j);
        triangle(c, shader, image, zbuf, vp, W, H);
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
    std::cout << "done" << std::endl;

    delete[] zbuf;
    return 0;
}