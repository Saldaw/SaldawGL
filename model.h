#pragma once
#include "geometry.h"
#include "tgaimage.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>

class Model {
    std::vector<Vec3f> verts;
    std::vector<Vec3f> norms;
    std::vector<Vec2f> uvs;
    std::vector<int> fv;
    std::vector<int> fn;
    std::vector<int> ft;
    TGAImage tex;

public:
    Model(std::string filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            std::cout << "cant open " << filename << std::endl;
            return;
        }
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;
            if (type == "v") {
                Vec3f v;
                iss >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }
            else if (type == "vn") {
                Vec3f n;
                iss >> n.x >> n.y >> n.z;
                norms.push_back(n);
            }
            else if (type == "vt") {
                Vec2f uv;
                iss >> uv.x >> uv.y;
                uvs.push_back(uv);
            }
            else if (type == "f") {
                std::vector<int> vv, tt, nn;
                std::string s;
                while (iss >> s) {
                    int v = 0, t = 0, n = 0;
                    size_t a = s.find('/');
                    if (a == std::string::npos) {
                        v = std::stoi(s);
                    }
                    else {
                        v = std::stoi(s.substr(0, a));
                        size_t b = s.find('/', a + 1);
                        if (b == std::string::npos) {
                            t = std::stoi(s.substr(a + 1));
                        }
                        else {
                            if (b > a + 1) t = std::stoi(s.substr(a + 1, b - a - 1));
                            n = std::stoi(s.substr(b + 1));
                        }
                    }
                    vv.push_back(v - 1);
                    tt.push_back(t - 1);
                    nn.push_back(n - 1);
                }
                for (size_t i = 1; i + 1 < vv.size(); i++) {
                    fv.push_back(vv[0]); fv.push_back(vv[i]); fv.push_back(vv[i + 1]);
                    ft.push_back(tt[0]); ft.push_back(tt[i]); ft.push_back(tt[i + 1]);
                    fn.push_back(nn[0]); fn.push_back(nn[i]); fn.push_back(nn[i + 1]);
                }
            }
        }

        std::string tname = filename.substr(0, filename.find_last_of(".")) + "_diffuse.tga";
        if (!tex.read_tga_file(tname.c_str())) {
            std::cout << "no texture " << tname << std::endl;
        }
        tex.flip_vertically();
    }

    int nfaces() { return fv.size() / 3; }

    Vec3f vert(int i, int j) { return verts[fv[i * 3 + j]]; }

    Vec3f normal(int i, int j) {
        if (fn.empty() || fn[i * 3 + j] < 0) return Vec3f(0, 0, 1);
        return norms[fn[i * 3 + j]];
    }

    Vec2f uv(int i, int j) {
        if (ft.empty() || ft[i * 3 + j] < 0) return Vec2f(0, 0);
        return uvs[ft[i * 3 + j]];
    }

    TGAColor diffuse(Vec2f uv) {
        if (tex.get_width() == 0) {
            return TGAColor(255, 255, 255, 255);
        }
        float u = uv.x - std::floor(uv.x);
        float v = uv.y - std::floor(uv.y);
        int x = std::max(0, std::min(tex.get_width() - 1, (int)(u * tex.get_width())));
        int y = std::max(0, std::min(tex.get_height() - 1, (int)((1.0f - v) * tex.get_height())));
        return tex.get(x, y);
    }
};