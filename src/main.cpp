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
#include "../include/menu.h"

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
    Shapes focal;
    // Configurar câmera
    Camera camera;
    // Configurar Renderer (iluminação e shadi  ng)
    Renderer renderer;

    Material material = MATERIAL_RUBBER;

    shapes.createCube(material, {0, 0, 0}, 2.0f); //Criar cubo
    focal.createSphere(material, camera.look, 0.04f, 6, 6); //Criar look Point


    

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

    app.setKeyCallback([&](int key, int, int action, int) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
        key_press(key,menu_type,shape_type,camera,material,currentMode,renderer);
    });

    app.setCursorPosCallback([&](double x, double y) {
        float dx = static_cast<float>(x - lastX);
        float dy = static_cast<float>(y - lastY);
        lastX = x;
        lastY = y;

        const float sensitivity = 0.5f;
        if(rmousepress){
            camera.addX(dx * sensitivity);   // esquerda/direita
            camera.addY(-dy * sensitivity);  // cima/baixo
        }
    });

    app.setScrollCallback([&](double /*xoffset*/, double yoffset) {
        const float zoomStep = 1.0f;
        camera.addOrbitDistance(-yoffset * zoomStep);
    });

    app.setMouseButtonCallback([&](int button, int action, int mods){
        if(button == GLFW_MOUSE_BUTTON_RIGHT){
            if(action == GLFW_PRESS){
                rmousepress = true;
            }
            else rmousepress = false;
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
        focal.objects[0].translate(camera.look-focal.objects[0].mesh.verts[0].position);

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

        //Desenhar ponto de visagem da camera
        for (auto& s : focal.objects) {
            for (const auto& face : s.mesh.faces) {
                Polygon poly2D = camera.projectAndClip(s.mesh, face, w, h);

                if (poly2D.verts.size() >= 3) {
                    // calcular iluminação baseada no modo de shading
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

                    fill_polygon(poly2D, fb, renderer);
                }
            }
        }

        menu(menu_type, shape_type, fb, camera, currentMode, material, fps);

                
        app.drawFramebuffer(fb.colorData(), fb.width(), fb.height());

        app.endFrame();
    }

    app.shutdown();
    return 0;
}
