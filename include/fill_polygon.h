#pragma once
#include "../include/types.h"
#include "../include/draw.h"
#include "../include/renderer.h"

struct frac {
    int top;
    int bottom;
};

struct node {
    int ymax;
    frac xmin;
    frac m_inv;
};

void fill_polygon(const Polygon& p, Framebuffer& fb, Renderer& renderer);
