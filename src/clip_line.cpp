#include "../include/clip_line.h"

// códigos de região
static const int INSIDE = 0;
static const int LEFT = 1;
static const int RIGHT = 2;
static const int BOTTOM = 4;
static const int TOP = 8;

static int computeOutCode(int x, int y, int xmin, int ymin, int xmax,
                          int ymax) {
    int code = INSIDE;

    if (x < xmin)
        code |= LEFT;
    else if (x > xmax)
        code |= RIGHT;

    if (y < ymin)
        code |= BOTTOM;
    else if (y > ymax)
        code |= TOP;

    return code;
}

bool clipLineCohenSutherland(Line2D& l, int xmin, int ymin, int xmax,
                             int ymax) {
    int x0 = l.x1, y0 = l.y1;
    int x1 = l.x2, y1 = l.y2;
    float z0 = l.z1, z1 = l.z2;

    int out0 = computeOutCode(x0, y0, xmin, ymin, xmax, ymax);
    int out1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);

    bool accept = false;

    while (true) {
        if ((out0 | out1) == 0) {
            // trivialmente aceito
            accept = true;
            break;
        } else if (out0 & out1) {
            // trivialmente rejeitado
            break;
        } else {
            // calcular interseção
            int out = out0 ? out0 : out1;
            float x, y, z;

            float t;  // interpolador z

            if (out & TOP) {
                y = ymax;
                t = float(ymax - y0) / (y1 - y0);
                x = x0 + t * (x1 - x0);
            } else if (out & BOTTOM) {
                y = ymin;
                t = float(ymin - y0) / (y1 - y0);
                x = x0 + t * (x1 - x0);
            } else if (out & RIGHT) {
                x = xmax;
                t = float(xmax - x0) / (x1 - x0);
                y = y0 + t * (y1 - y0);
            } else {  // LEFT
                x = xmin;
                t = float(xmin - x0) / (x1 - x0);
                y = y0 + t * (y1 - y0);
            }

            z = z0 + t * (z1 - z0);

            if (out == out0) {
                x0 = (int)x;
                y0 = (int)y;
                z0 = z;
                out0 = computeOutCode(x0, y0, xmin, ymin, xmax, ymax);
            } else {
                x1 = (int)x;
                y1 = (int)y;
                z1 = z;
                out1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            }
        }
    }

    if (accept) {
        l.x1 = x0;
        l.y1 = y0;
        l.z1 = z0;
        l.x2 = x1;
        l.y2 = y1;
        l.z2 = z1;
    }

    return accept;
}
