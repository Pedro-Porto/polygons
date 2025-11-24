#include "../include/draw.h"
#include <cmath>

void write_pixel_dilated(int x, int y, Color color, int width, Framebuffer& fb) {
    int manhd;
    int xmid = MAX_WIDTH - 1, ymid = MAX_WIDTH - 1;

    for (int i = 0; i < 2 * MAX_WIDTH - 1; i++) {
        for (int j = 0; j < 2 * MAX_WIDTH - 1; j++) {
            manhd = abs(i - xmid) + abs(j - ymid);
            if (manhd <= width)
                write_pixel(x + i - xmid, y + j - ymid, color, fb);
        }
    }
}
