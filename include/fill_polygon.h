#pragma once
#include "../include/types.h"
#include "../include/draw.h"

struct frac {
    int top;
    int bottom;
};

struct node {
    int ymax;
    frac xmin;
    frac m_inv;
};

void fill_polygon(polygon p, unsigned int screen_height, Framebuffer& fb) ;
