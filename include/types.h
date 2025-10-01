#pragma once
#include <vector>

struct Color {
    unsigned char r, g, b, a;
};

struct vertex {
    int x, y;
};

struct line {
    vertex v1, v2;
    Color color;
    int width;
};

struct polygon {
    std::vector<line> walls;
    Color color;
    std::vector<float> gFillVerts;
    bool filled = false;
};