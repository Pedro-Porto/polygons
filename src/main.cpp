#include <iostream>
#include <vector>

#include "../include/draw.h"
#include "../include/gl_app.h"
#include "../include/polygon.h"

extern std::vector<float> gVerts;

static Color linecolor = {255, 255, 255, 255};
static Color polygoncolor = {255, 0, 0, 255};
static int linewidth = 3;

static bool drawing = false;
static vertex v;
static line l;
static polygon p;
static std::vector<polygon> polygons;

int main() {
    GLApp app;
    GLApp::Config cfg;
    cfg.width = 900;
    cfg.height = 600;
    cfg.title = "Polygons";
    if (!app.init(cfg)) return -1;

    app.setCursorPosCallback([](double mx, double my) {
        v.x = (int)mx;
        v.y = (int)my;
        if (drawing && !p.walls.empty()) {
            p.walls.back().v2 = v;
            p.walls.back().color = linecolor;
            p.walls.back().width = linewidth;
        }
    });

    app.setMouseButtonCallback([](int button, int action, int) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            drawing = true;
            l.v1 = v;
            l.v2 = v;
            l.color = linecolor;
            l.width = linewidth;
            p.walls.push_back(l);
        }
    });

    app.setKeyCallback([](int key, int, int action, int) {
        if (action != GLFW_PRESS) return;
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
                break;
            case GLFW_KEY_SPACE:
                if (!p.walls.empty()) {
                    drawing = false;
                    p.walls.back().v2 = p.walls.front().v1;
                    polygons.push_back(p);
                    p.walls.clear();
                }
                break;
            case GLFW_KEY_Z:
                if (linewidth >= 2) linewidth--;
                break;
            case GLFW_KEY_X:
                if (linewidth < 10) linewidth++;
                break;
            case GLFW_KEY_R:
                linecolor = {255, 0, 0, 255};
                break;
            case GLFW_KEY_G:
                linecolor = {0, 255, 0, 255};
                break;
            case GLFW_KEY_B:
                linecolor = {0, 0, 255, 255};
                break;
            case GLFW_KEY_W:
                linecolor = {255, 255, 255, 255};
                break;
        }
    });

    l.color = linecolor;
    l.width = linewidth;
    p.color = polygoncolor;

    while (!glfwWindowShouldClose(app.window())) {
        app.beginFrame();

        gVerts.clear();
        for (const auto& poly : polygons) draw_polygon_walls(poly);
        draw_polygon_walls(p);

        app.drawPoints(gVerts);
        app.endFrame();
    }

    app.shutdown();
    return 0;
}
