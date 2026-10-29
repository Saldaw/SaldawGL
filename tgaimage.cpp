#include "tgaimage.h"
#include <cstring>

bool TGAImage::read_tga_file(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;

    TGAHeader header;
    in.read((char*)&header, sizeof(header));

    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3;

    if (width <= 0 || height <= 0 || (bytespp != 3 && bytespp != 4)) return false;

    data.resize(width * height * bytespp);

    if (header.datatypecode == 2) {
        in.read((char*)data.data(), data.size());
    }
    else if (header.datatypecode == 10) {
        int total = width * height;
        int curpix = 0;
        int curbyte = 0;
        while (curpix < total) {
            unsigned char chunk = 0;
            in.read((char*)&chunk, 1);
            if (chunk < 128) {
                chunk++;
                for (int i = 0; i < chunk; i++) {
                    in.read((char*)&data[curbyte], bytespp);
                    curbyte += bytespp;
                    curpix++;
                }
            }
            else {
                chunk -= 127;
                unsigned char col[4];
                in.read((char*)col, bytespp);
                for (int i = 0; i < chunk; i++) {
                    for (int b = 0; b < bytespp; b++) data[curbyte++] = col[b];
                    curpix++;
                }
            }
        }
    }
    else {
        return false;
    }

    in.close();
    if (header.imagedescriptor & 0x20) flip_vertically();
    return true;
}

bool TGAImage::write_tga_file(const char* filename) {
    TGAHeader header = { 0, 0, 2, 0, 0, 0, 0, 0, (short)width, (short)height, (char)(bytespp << 3), 0x20 };
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;
    out.write((char*)&header, sizeof(header));
    out.write((char*)data.data(), data.size());
    out.close();
    return true;
}

void TGAImage::flip_vertically() {
    int line = width * bytespp;
    std::vector<unsigned char> tmp(line);
    for (int y = 0; y < height / 2; y++) {
        memcpy(tmp.data(), &data[y * line], line);
        memcpy(&data[y * line], &data[(height - 1 - y) * line], line);
        memcpy(&data[(height - 1 - y) * line], tmp.data(), line);
    }
}