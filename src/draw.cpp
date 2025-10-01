#include "../include/draw.h"

std::vector<float> gVerts;

static inline void colorToFloats(Color c, float& r, float& g, float& b,
                                 float& a) {
    r = c.r / 255.0f;
    g = c.g / 255.0f;
    b = c.b / 255.0f;
    a = c.a / 255.0f;
}

void write_pixel(int x, int y, Color color) {
    float r, g, b, a;
    colorToFloats(color, r, g, b, a);
    gVerts.push_back((float)x);
    gVerts.push_back((float)y);
    gVerts.push_back(r);
    gVerts.push_back(g);
    gVerts.push_back(b);
    gVerts.push_back(a);
}

void write_pixel_dilated(int x, int y, Color color, int width) {
    int manhd;
    int xmid = MAX_WIDTH - 1, ymid = MAX_WIDTH - 1;

    for (int i = 0; i < 2 * MAX_WIDTH - 1; i++) {
        for (int j = 0; j < 2 * MAX_WIDTH - 1; j++) {
            manhd = abs(i - xmid) + abs(j - ymid);
            if (manhd <= width) write_pixel(x + i - xmid, y + j - ymid, color);
        }
    }
}
