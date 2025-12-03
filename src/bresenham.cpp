#include "../include/bresenham.h"

void plot_line(const Line2D& l, Framebuffer& fb) {
    // definição da reta
    int xi = l.x1;
    int yi = l.y1;
    int xf = l.x2;
    int yf = l.y2;

    float zi = l.z1;
    float zf = l.z2;

    int px = (xf >= xi) ? 1 : -1; // passo em x 
    int py = (yf >= yi) ? 1 : -1; // passo em y

    // calculos
    int dy = abs(yf - yi);
    int dx = abs(xf - xi);
    // verificando se > 45 (se dx > dy é menor que 45)
    int deltaE, deltaNE, d;
    int up45 = 0;
    int down45 = 0;
    if (dx > dy) { // menor que 45
        deltaE = 2 * dy;
        deltaNE = 2 * (dy - dx);
        d = (2 * dy) - dx;
        down45 = 1;
    }
    else { // maior que 45
        deltaE = 2 * dx;
        deltaNE = 2 * (dx - dy);
        d = (2 * dx) - dy;
        up45 = 1;
    }
    int x = xi;
    int y = yi;

    // número TOTAL de passos (sem contar o primeiro pixel)
    int stepsTotal = down45 ? dx : dy;
    int steps = stepsTotal;

    // primeiro ponto: t = 0 → z = zi
    float z = zi;
    write_pixel_dilated_z(x, y, z, l.color, l.width, fb);

    int stepIndex = 0; // quantos passos já demos

    while (steps--) {
        if (d < 0) { // escolhe E
            d = d + deltaE;
            x += px * down45;
            y += py * up45;
        }
        else { // escolhe NE
            d = d + deltaNE;
            x += px;
            y += py;
        }

        // avança o índice de passo
        stepIndex++;

        // t vai de 0 → 1 ao longo da linha
        float t = (stepsTotal > 0) ? (float)stepIndex / (float)stepsTotal : 0.0f;
        z = zi + (zf - zi) * t;

        write_pixel_dilated_z(x, y, z, l.color, l.width, fb);
    }
}
