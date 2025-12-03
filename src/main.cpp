#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include "../include/camera.h"
#include "../include/fill_polygon.h"
#include "../include/font8x8.h"
#include "../include/framebuffer.h"
#include "../include/gl_app.h"
#include "../include/renderer.h"
#include "../include/shapes.h"
#include "../include/types.h"


int main() {
    GLApp app;
    GLApp::Config cfg;
    cfg.width = 900;
    cfg.height = 600;
    cfg.title = "renderer";
    if (!app.init(cfg)) return -1;

    // Framebuffer CPU (cor + z-buffer)
    Framebuffer fb(cfg.width, cfg.height);

    // Criar cubo
    Shapes shapes;
    // Configurar câmera
    Camera camera;
    // Configurar Renderer (iluminação e shading)
    Renderer renderer;

    shapes.createCube({0, 0, 0}, 2.0f);

    initFont8x8();

    double lastTime = glfwGetTime();
    int framesThisSecond = 0;
    int fps = 0;

    Light light;
    light.pos = glm::vec3(5, 5, 5);
    light.color = glm::vec3(1, 1, 1);
    light.intensity = 1.0f;
    renderer.setLight(light);

    ShadingMode currentMode = ShadingMode::Flat;
    renderer.setMode(currentMode);

    float lastX = 0.0f, lastY = 0.0f;

    app.setKeyCallback([&](int key, int, int action, int) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

        const float moveStep = 0.2f;

        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
                break;

            case GLFW_KEY_W:
                camera.moveZ(-moveStep);
                break;
            case GLFW_KEY_S:
                camera.moveZ(moveStep);
                break;
            case GLFW_KEY_A:
                camera.moveX(-moveStep);
                break;
            case GLFW_KEY_D:
                camera.moveX(moveStep);
                break;
            case GLFW_KEY_Q:
                camera.moveY(moveStep);
                break;
            case GLFW_KEY_E:
                camera.moveY(-moveStep);
                break;

            case GLFW_KEY_1:
                currentMode = ShadingMode::Flat;
                renderer.setMode(currentMode);
                std::cout << "Modo: Flat Shading\n";
                break;
            case GLFW_KEY_2:
                currentMode = ShadingMode::Gouraud;
                renderer.setMode(currentMode);
                std::cout << "Modo: Gouraud Shading\n";
                break;
            case GLFW_KEY_3:
                currentMode = ShadingMode::Phong;
                renderer.setMode(currentMode);
                std::cout << "Modo: Phong Shading\n";
                break;
        }
    });

    app.setCursorPosCallback([&](double x, double y) {
        float dx = static_cast<float>(x - lastX);
        float dy = static_cast<float>(y - lastY);
        lastX = x;
        lastY = y;

        const float sensitivity = 0.5f;
        camera.addX(dx * sensitivity);   // esquerda/direita
        camera.addY(-dy * sensitivity);  // cima/baixo
    });

    app.setScrollCallback([&](double /*xoffset*/, double yoffset) {
        const float zoomStep = 1.0f;
        camera.addOrbitDistance(-yoffset * zoomStep);
    });

    while (!glfwWindowShouldClose(app.window())) {
        double currentTime = glfwGetTime();
        framesThisSecond++;

        if (currentTime - lastTime >= 1.0) {
            fps = framesThisSecond;
            framesThisSecond = 0;
            lastTime = currentTime;
        }
        app.beginFrame();

        // redimensionar framebuffer se mudar o tamanho da janela
        int w, h;
        glfwGetFramebufferSize(app.window(), &w, &h);
        if (w != fb.width() || h != fb.height()) {
            fb.resize(w, h);
        }

        // limpar buffers
        fb.clear({30, 30, 40, 255});
        fb.clearDepth(1000.0f);

        // atualizar posição da câmera no renderer (para Phong)
        renderer.setCameraEye(camera.eye);

        for (auto& s : shapes.objects) {
            for (const auto& face : s.mesh.faces) {
                Polygon poly2D = camera.projectAndClip(s.mesh, face, w, h);

                if (poly2D.verts.size() >= 3) {
                    // calcular iluminação baseada no modo de shading

                    if (currentMode == ShadingMode::Flat) {
                        // FLAT: calcular intensidade uma vez por face usando o
                        // centro
                        glm::vec3 faceCenter(0);
                        glm::vec3 faceNormal(0);
                        for (const auto& v : poly2D.verts) {
                            faceCenter += glm::vec3(v.x, v.y, v.z);
                            faceNormal += v.normal;
                        }
                        faceCenter /= float(poly2D.verts.size());
                        faceNormal = glm::normalize(faceNormal);

                        float intensity = renderer.phong(faceCenter, faceNormal,
                                                         *poly2D.material);
                        renderer.setFlatIntensity(intensity);

                    } else if (currentMode == ShadingMode::Gouraud) {
                        // GOURAUD: calcular intensidade por vértice
                        for (auto& v : poly2D.verts) {
                            glm::vec3 pos3D(v.x, v.y, v.z);
                            v.intensity = renderer.phong(pos3D, v.normal,
                                                         *poly2D.material);
                        }
                    }
                    // PHONG: intensidade será calculada por pixel no scanline

                    fill_polygon(poly2D, fb, renderer);
                }
            }
        }

        Color hudColor = {220, 220, 220, 255};

        int fontScale = 2;
        int lineH = 8 * fontScale + 4;  // altura da linha com margin

        drawText(
            fb, 10, 10,
            std::string("PROJ: ") +
                (camera.type == Camera::ProjType::Perspective ? "PERSPECTIVE"
                                                              : "ORTHO"),
            hudColor, fontScale);

        drawText(fb, 10, 10 + lineH,
                 std::string("SHADING: ") +
                     (currentMode == ShadingMode::Flat      ? "FLAT"
                      : currentMode == ShadingMode::Gouraud ? "GOURAUD"
                                                            : "PHONG"),
                 hudColor, fontScale);

        drawText(
            fb, 10, 10 + 2 * lineH,
            std::string("CAMERA: ") +
                (camera.moveType == Camera::MoveType::Orbit ? "ORBIT" : "FPS"),
            hudColor, fontScale);

        drawText(fb, 10, 10 + 3 * lineH,
                 std::string("FPS: ") + std::to_string(fps), hudColor,
                 fontScale);

        app.drawFramebuffer(fb.colorData(), fb.width(), fb.height());

        app.endFrame();
    }

    app.shutdown();
    return 0;
}
