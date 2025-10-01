#include "../include/polygon.h"

void draw_polygon_walls(polygon p) {
    for (auto it = p.walls.begin(); it < p.walls.end(); it++) {
        plot_line(*it);
    }
}