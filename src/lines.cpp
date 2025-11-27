#include "../include/lines.h"

// void plot_line(line l, Framebuffer& fb) {
//     // definição da reta
//     int xi = l.v1.x;
//     int yi = l.v1.y;
//     int xf = l.v2.x;
//     int yf = l.v2.y;

//     int px = (xf >= xi) ? 1 : -1; // passo em x 
//     int py = (yf >= yi) ? 1 : -1; // passo em y

//     // calculos
//     int dy = abs(yf - yi);
//     int dx = abs(xf - xi);
//     // verificando se > 45 (se dx > dy é menor que 45)
//     int deltaE, deltaNE, d;
//     int up45 = 0;
//     int down45 = 0;
//     if (dx > dy) { // menor que 45
//         deltaE = 2 * dy;
//         deltaNE = 2 * (dy - dx);
//         d = (2 * dy) - dx;
//         down45 = 1;
//     }
//     else { // maior que 45
//         deltaE = 2 * dx;
//         deltaNE = 2 * (dx - dy);
//         d = (2 * dx) - dy;
//         up45 = 1;
//     }
//     int x = xi;
//     int y = yi;
//     write_pixel_dilated(x, y, l.color, l.width, fb);

//     int steps = down45 ? dx : dy;

//     while (steps--) {
//         if (d < 0) { // escolhe E
//             d = d + deltaE;
//             x += px * down45;
//             y += py * up45;
//         }
//         else { // escolhe NE
//             d = d + deltaNE;
//             x += px;
//             y += py;
//         }
//         write_pixel_dilated(x, y, l.color, l.width, fb);
//     }
// }