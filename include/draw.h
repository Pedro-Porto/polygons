#ifndef DRAW_H
#define DRAW_H

#include "types.h"
#include "framebuffer.h"

#define MAX_WIDTH 10

static inline void write_pixel(int x, int y, Color c, Framebuffer& fb) {
    if (x < 0 || y < 0 || x >= fb.width() || y >= fb.height()) return;
    fb.color(x, y) = (uint32_t(c.r) << 24) | (uint32_t(c.g) << 16) |
                     (uint32_t(c.b) << 8)  | uint32_t(c.a);
}

static inline int write_pixel_z(int x, int y, float z, Color c, Framebuffer& fb) {
    return fb.set(x, y, z, c);
}

void write_pixel_dilated(int x, int y, Color c, int width, Framebuffer& fb);

#endif
