#include <iostream>
#include <vector>

#include "../include/draw.h"
#include "../include/fill_polygon.h"
#include "../include/gl_app.h"
#include "../include/polygon.h"
#include "../include/framebuffer.h"

static Color linecolor   = {255, 255, 255, 255};
static Color polygoncolor= {255,   255,   0, 255};
static int linewidth = 3;

static bool drawing = false;
static vertex v;
static line l;
static polygon p;
static std::vector<polygon> polygons;

int main() {
    GLApp app;
    GLApp::Config cfg;
    cfg.width  = 900;
    cfg.height = 600;
    cfg.title  = "Polygons (Framebuffer)";
    if (!app.init(cfg)) return -1;

    // framebuffer CPU (cor + z)
    Framebuffer fb(cfg.width, cfg.height);

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

    app.setKeyCallback([&cfg](int key, int, int action, int) {
        if (action != GLFW_PRESS) return;
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
                break;

            case GLFW_KEY_SPACE:
                if (!p.walls.empty()) {
                    drawing = false;
                    p.walls.back().v2 = p.walls.front().v1; // fecha polígono
                    p.color = polygoncolor;
                    p.filled = true;
                    polygons.push_back(p);

                    // prepara novo polígono
                    p.filled = false;
                    p.walls.clear();
                }
                break;

            case GLFW_KEY_BACKSPACE:
                p.walls.clear();
                p.filled = false;
                polygons.clear();
                break;

            case GLFW_KEY_Z:
                if (linewidth >= 2) linewidth--;
                break;

            case GLFW_KEY_X:
                if (linewidth < 10) linewidth++;
                break;

            case GLFW_KEY_R:
                polygoncolor = {255, 0, 0, 255};
                break;
            case GLFW_KEY_G:
                polygoncolor = {0, 255, 0, 255};
                break;
            case GLFW_KEY_B:
                polygoncolor = {0, 0, 255, 255};
                break;
            case GLFW_KEY_W:
                polygoncolor = {255, 255, 255, 255};
                break;
        }
    });

    l.color = linecolor;
    l.width = linewidth;
    p.color = polygoncolor;

    while (!glfwWindowShouldClose(app.window())) {
        app.beginFrame();

        // se a janela mudar de tamanho, redimensiona o fb
        int w, h;
        glfwGetFramebufferSize(app.window(), &w, &h);
        if (w != fb.width() || h != fb.height()) {
            fb.resize(w, h);
        }

        // limpa buffers CPU
        fb.clear({0,0,0,255});
        fb.clearDepth(1e9f);

        // rasteriza todos os polígonos já fechados
        for (const auto& poly : polygons) {
            if (poly.filled) {
                fill_polygon(poly, (unsigned int)fb.height(), fb); // <<< agora escreve no fb
            }
            draw_polygon_walls(poly, fb); // <<< walls no fb (wireframe)
        }

        // desenha o polígono em construção (só walls)
        draw_polygon_walls(p, fb);

        // OpenGL só mostra a imagem pronta do framebuffer
        app.drawFramebuffer(fb.colorData(), fb.width(), fb.height());

        app.endFrame();
    }

    app.shutdown();
    return 0;
}
