#include <clip_line.h>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "../include/bresenham.h"
#include "../include/camera.h"
#include "../include/extrusion.h"
#include "../include/fill_polygon.h"
#include "../include/font8x8.h"
#include "../include/framebuffer.h"
#include "../include/gl_app.h"
#include "../include/lines.h"
#include "../include/renderer.h"
#include "../include/shapes.h"
#include "../include/types.h"
#include "../include/menu.h"

// --------- helper: desenhar linha 3D -----

static void drawLine3D(const Line3D& l3, const Camera& camera, Framebuffer& fb,
                       int W, int H) {
    Line2D l2;
    if (!camera.projectLine(l3, l2, W, H)) return;
    if (!clipLineCohenSutherland(l2, 0, 0, W - 1, H - 1)) return;
    plot_line(l2, fb);
}

// ================== MAIN ==================

int main() {
    GLApp app;
    GLApp::Config cfg;
    cfg.width = 900;
    cfg.height = 600;
    cfg.title = "renderer";
    if (!app.init(cfg)) return -1;

    // Framebuffer CPU (cor + z-buffer)
    Framebuffer fb(cfg.width, cfg.height);

    // Criar Figuras
    Shapes shapes;
    Lines lines;
    Lines lookGizmo;
    // Configurar câmera
    Camera camera;
    // Configurar Renderer (iluminação e shadi  ng)
    Renderer renderer;

    Material material = MATERIAL_RUBBER;

    shapes.createCube(material, {0, 0, 0}, 2.0f);  // Criar cubo

    lines.add({-5, 0, 0}, {5, 0, 0}, {255, 0, 0, 255}, 1);  // eixo X
    lines.add({0, -5, 0}, {0, 5, 0}, {0, 255, 0, 255}, 1);  // eixo Y
    lines.add({0, 0, -5}, {0, 0, 5}, {0, 0, 255, 255}, 1);  // eixo Z

    float g = 0.5f;  // tamanho do gizmo
    lookGizmo.add({-g, 0, 0}, {g, 0, 0}, {255, 0, 0, 255}, 1);  // X local
    lookGizmo.add({0, -g, 0}, {0, g, 0}, {0, 255, 0, 255}, 1);  // Y local
    lookGizmo.add({0, 0, -g}, {0, 0, g}, {0, 0, 255, 255}, 1);  // Z local

    initFont8x8();

    double lastTime = glfwGetTime();
    int framesThisSecond = 0;
    int fps = 0;
    MenuType menu_type = MenuType::Camera;
    ShapeType shape_type = ShapeType::Cube;

    bool rmousepress = false;

    Light light;
    light.pos = glm::vec3(5, 5, 5);
    light.color = glm::vec3(1, 1, 1);
    light.intensity = 1.0f;
    renderer.setLight(light);

    ShadingMode currentMode = ShadingMode::Flat;
    renderer.setMode(currentMode);

    float lastX = 0.0f, lastY = 0.0f;

    // ===== estado para desenho/extrusão =====
    ExtrusionState extrusionState;
    double mouseX = 0.0, mouseY = 0.0;

    app.setKeyCallback([&](int key, int, int action, int) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

        const float moveStep = 0.2f;

        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
                break;

            case GLFW_KEY_W:
                camera.moveZ(moveStep);
                break;
            case GLFW_KEY_S:
                camera.moveZ(-moveStep);
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
            case GLFW_KEY_Z:
                shapes.createSphere(material, camera.look, 1.5, 12, 24);
                break;
            case GLFW_KEY_X:
                shapes.createCylinder(material, camera.look, 1.5, 2.0, 16);
                break;
            case GLFW_KEY_C:
                shapes.createCube(material, camera.look, 2.0);
                break;
            case GLFW_KEY_V:
                shapes.createPyramid(material, camera.look, 1.5, 1.5);
                break;
            case GLFW_KEY_Y:
                // select_color(material, fb);
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
            case GLFW_KEY_7:
                camera.type = Camera::ProjType::Perspective;
                std::cout << "Camera Projection: Perspective\n";
                break;
            case GLFW_KEY_8:
                camera.type = Camera::ProjType::Ortho;
                std::cout << "Camera Projection: Orthogonal\n";
                break;
            case GLFW_KEY_9:
                camera.moveType = Camera::MoveType::Fps;
                std::cout << "Camera Mode: Fps\n";
                break;
            case GLFW_KEY_0:
                camera.moveType = Camera::MoveType::Orbit;
                std::cout << "Camera mode: Orbit\n";
                break;

            case GLFW_KEY_O:
                extrusionState.mode = EditMode::Draw;
                extrusionState.polygon3D.clear();
                extrusionState.hasPreview = false;
                break;

            case GLFW_KEY_T:
                camera.eye = glm::vec3(0, 0, 10);
                camera.look = glm::vec3(0, 0, 0);
                camera.up = glm::vec3(0, 1, 0);
                break;

            case GLFW_KEY_P:
                if (extrusionState.plane == DrawPlane::XY)
                    extrusionState.plane = DrawPlane::XZ;
                else if (extrusionState.plane == DrawPlane::XZ)
                    extrusionState.plane = DrawPlane::YZ;
                else
                    extrusionState.plane = DrawPlane::XY;
                std::cout << "Plano: "
                          << getPlaneNameString(extrusionState.plane) << "\n";
                break;

            case GLFW_KEY_SPACE:
                if (extrusionState.mode == EditMode::Draw &&
                    extrusionState.polygon3D.size() >= 3) {
                    extrusionState.mode = EditMode::Extrude;
                    extrusionState.extrudeDepth = 0.0f;
                    extrusionState.extrudeStartPoint =
                        extrusionState.polygon3D[0];
                } else if (extrusionState.mode == EditMode::Extrude) {
                    buildExtrudedSolid(
                        extrusionState.polygon3D, extrusionState.plane,
                        extrusionState.extrudeDepth, shapes, material);
                    extrusionState.mode = EditMode::None;
                    extrusionState.polygon3D.clear();
                    extrusionState.hasPreview = false;
                }
                break;
        }
        key_press(key,menu_type,shape_type,camera,material,currentMode,renderer);
    });

    app.setCursorPosCallback([&](double x, double y) {
        float dx = static_cast<float>(x - lastX);
        float dy = static_cast<float>(y - lastY);
        lastX = x;
        lastY = y;

        mouseX = x;
        mouseY = y;

        const float sensitivity = 0.5f;
        if (rmousepress) {
            camera.addX(dx * sensitivity);   // esquerda/direita
            camera.addY(-dy * sensitivity);  // cima/baixo
        }
    });

    app.setScrollCallback([&](double /*xoffset*/, double yoffset) {
        const float zoomStep = 1.0f;
        camera.addOrbitDistance(-yoffset * zoomStep);
    });

    app.setMouseButtonCallback([&](int button, int action, int /*mods*/) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                rmousepress = true;
            } else
                rmousepress = false;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            if (extrusionState.mode == EditMode::Draw &&
                extrusionState.hasPreview) {
                extrusionState.polygon3D.push_back(extrusionState.previewPoint);
                std::cout << "Vértice adicionado ("
                          << extrusionState.polygon3D.size() << " total)\n";
            }
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            switch (shape_type){
                case ShapeType::Cube:
                shapes.createCube(material, camera.look,2.0);
                break;
                case ShapeType::Sphere:
                shapes.createSphere(material, camera.look,1.5,12,24);
                break;
                case ShapeType::Cylinder:
                shapes.createCylinder(material, camera.look,1.5,2.0,16);
                break;
                case ShapeType::Pyramid:
                shapes.createPyramid(material, camera.look,1.5,1.5);
                break;
            }
        }

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

        // atualizar preview do ponto no plano de desenho
        if (extrusionState.mode != EditMode::None) {
            updateExtrusionPreview(extrusionState, mouseX, mouseY, w, h,
                                   camera);
        }

        // desenhar sólidos
        for (auto& s : shapes.objects) {
            for (const auto& face : s.mesh.faces) {
                Polygon poly2D = camera.projectAndClip(s.mesh, face, w, h);

                if (poly2D.verts.size() >= 3) {
                    if (currentMode == ShadingMode::Flat) {
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
                        for (auto& v : poly2D.verts) {
                            glm::vec3 pos3D(v.x, v.y, v.z);
                            v.intensity = renderer.phong(pos3D, v.normal,
                                                         *poly2D.material);
                        }
                    }

                    fill_polygon(poly2D, fb, renderer);
                }
            }
        }

        // desenha as linhas dos eixos globais
        for (const auto& l3 : lines.objects) {
            Line2D l2;

            if (!camera.projectLine(l3, l2, w, h)) continue;

            if (!clipLineCohenSutherland(l2, 0, 0, w - 1, h - 1)) continue;

            plot_line(l2, fb);
        }

        // gizmo do look
        for (const auto& base : lookGizmo.objects) {
            Line3D worldLine = base;
            worldLine.p1 += camera.look;
            worldLine.p2 += camera.look;

            Line2D l2;
            if (camera.projectLine(worldLine, l2, w, h)) {
                plot_line(l2, fb);
            }
        }

        // desenhar polígono no modo draw (linhas amarelas)
        if (extrusionState.mode == EditMode::Draw) {
            // Desenha as linhas do polígono
            if (!extrusionState.polygon3D.empty()) {
                for (int i = 1; i < (int)extrusionState.polygon3D.size(); ++i) {
                    Line3D l{extrusionState.polygon3D[i - 1],
                             extrusionState.polygon3D[i],
                             {255, 255, 0, 255},
                             2};
                    drawLine3D(l, camera, fb, w, h);
                }

                // Linha de preview do último vértice até o mouse
                if (extrusionState.hasPreview) {
                    Line3D l{extrusionState.polygon3D.back(),
                             extrusionState.previewPoint,
                             {255, 255, 255, 255},
                             2};
                    drawLine3D(l, camera, fb, w, h);
                }
            }
        }

        // renderizar preview da extrusão como poliedro sólido
        if (extrusionState.mode == EditMode::Extrude &&
            extrusionState.polygon3D.size() >= 3) {
            // Cria material cyan para preview
            Material previewMat = material;
            previewMat.color = {50, 200, 200, 255};  // Cyan

            // Cria o poliedro temporário
            Polyhedron previewPoly = createExtrudedPolyhedron(
                extrusionState.polygon3D, extrusionState.plane,
                extrusionState.extrudeDepth, previewMat);

            // Renderiza o poliedro de preview
            for (const auto& face : previewPoly.faces) {
                Polygon poly2D = camera.projectAndClip(previewPoly, face, w, h);

                if (poly2D.verts.size() >= 3) {
                    if (currentMode == ShadingMode::Flat) {
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
                        for (auto& v : poly2D.verts) {
                            glm::vec3 pos3D(v.x, v.y, v.z);
                            v.intensity = renderer.phong(pos3D, v.normal,
                                                         *poly2D.material);
                        }
                    }

                    fill_polygon(poly2D, fb, renderer);
                }
            }
        }

        menu(menu_type, shape_type, fb, camera, currentMode, material, fps);

        int hudY = 10 + 4 * lineH;
        if (extrusionState.mode == EditMode::None) {
            drawText(fb, 10, hudY, "O: DRAW POLYGON   P: CHANGE PLANE",
                     hudColor, fontScale);
        } else if (extrusionState.mode == EditMode::Draw) {
            drawText(fb, 10, hudY, "LMB: ADD VERTEX   SPACE: START EXTRUDE",
                     hudColor, fontScale);

            std::string info = std::string("PLANE: ") +
                               getPlaneNameString(extrusionState.plane) +
                               "   VERTICES: " +
                               std::to_string(extrusionState.polygon3D.size());

            drawText(fb, 10, hudY + lineH, info, hudColor, fontScale);
        } else if (extrusionState.mode == EditMode::Extrude) {
            drawText(fb, 10, hudY, "MOVE MOUSE TO SET HEIGHT   SPACE: CONFIRM",
                     hudColor, fontScale);

            char depthStr[64];
            std::snprintf(depthStr, sizeof(depthStr), "HEIGHT: %.2f",
                          extrusionState.extrudeDepth);
            drawText(fb, 10, hudY + lineH, depthStr, hudColor, fontScale);
        }

        app.drawFramebuffer(fb.colorData(), fb.width(), fb.height());

        app.endFrame();
    }

    app.shutdown();
    return 0;
}
