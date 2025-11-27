#include <iostream>
#include <vector>
#include <cmath>

#include "../include/gl_app.h"
#include "../include/framebuffer.h"
#include "../include/fill_polygon.h"
#include "../include/camera.h"
#include "../include/types.h"
#include "../include/renderer.h"

// Função para criar um cubo
Polyhedron createCube() {
    Polyhedron cube;
    
    // Definir vértices do cubo (-1 a 1)
    std::vector<glm::vec3> positions = {
        {-1, -1, -1}, // 0
        { 1, -1, -1}, // 1
        { 1,  1, -1}, // 2
        {-1,  1, -1}, // 3
        {-1, -1,  1}, // 4
        { 1, -1,  1}, // 5
        { 1,  1,  1}, // 6
        {-1,  1,  1}  // 7
    };
    
    // Calcular normais para cada face
    std::vector<glm::vec3> faceNormals = {
        { 0,  0, -1}, // frente
        { 0,  0,  1}, // trás
        {-1,  0,  0}, // esquerda
        { 1,  0,  0}, // direita
        { 0, -1,  0}, // baixo
        { 0,  1,  0}  // topo
    };
    
    // Criar vértices com normais
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex3D v;
        v.position = positions[i];
        v.normal = glm::vec3(0); // será calculada pela média das faces adjacentes
        cube.verts.push_back(v);
    }
    
    // Calcular normais dos vértices (média das faces adjacentes)
    std::vector<std::vector<int>> vertexFaces = {
        {0, 2, 4}, // 0
        {0, 3, 4}, // 1
        {0, 3, 5}, // 2
        {0, 2, 5}, // 3
        {1, 2, 4}, // 4
        {1, 3, 4}, // 5
        {1, 3, 5}, // 6
        {1, 2, 5}  // 7
    };
    
    for (size_t i = 0; i < cube.verts.size(); i++) {
        glm::vec3 n(0);
        for (int faceIdx : vertexFaces[i]) {
            n += faceNormals[faceIdx];
        }
        cube.verts[i].normal = glm::normalize(n);
    }
    
    // Definir faces (sentido anti-horário quando visto de fora)
    cube.faces = {
        {{0, 1, 2, 3}}, // frente  (z = -1)
        {{5, 4, 7, 6}}, // trás    (z = +1)
        {{4, 0, 3, 7}}, // esquerda (x = -1)
        {{1, 5, 6, 2}}, // direita  (x = +1)
        {{4, 5, 1, 0}}, // baixo    (y = -1)
        {{3, 2, 6, 7}}  // topo     (y = +1)
    };
    
    // Material colorido para cada face
    cube.material.color = {100, 150, 255, 255}; // azul claro
    
    return cube;
}

int main() {
    GLApp app;
    GLApp::Config cfg;
    cfg.width  = 900;
    cfg.height = 600;
    cfg.title  = "3D Cube Renderer - CPU Pipeline";
    if (!app.init(cfg)) return -1;

    // Framebuffer CPU (cor + z-buffer)
    Framebuffer fb(cfg.width, cfg.height);
    
    // Criar cubo
    Polyhedron cube = createCube();
    
    // Configurar câmera
    Camera camera;
    camera.type = Camera::ProjType::Perspective;
    camera.eye = glm::vec3(0, 0, 5);  // câmera no eixo Z olhando para origem
    camera.look = glm::vec3(0, 0, 0);
    camera.up = glm::vec3(0, 1, 0);
    camera.fovY = 60.0f;
    camera.nearp = 0.1f;
    camera.farp = 100.0f;
    
    // Configurar Renderer (iluminação e shading)
    Renderer renderer;
    
    // Configurar luz
    Light light;
    light.pos = glm::vec3(5, 5, 5);      // luz posicionada acima e ao lado
    light.color = glm::vec3(1, 1, 1);    // luz branca
    light.intensity = 1.0f;
    renderer.setLight(light);
    
    // Modo de shading inicial (pode ser mudado com teclas)
    ShadingMode currentMode = ShadingMode::Flat;
    renderer.setMode(currentMode);
    
    float rotation = 0.0f;
    
    // Callbacks para interação
    app.setKeyCallback([&](int key, int, int action, int) {
        if (action != GLFW_PRESS) return;
        
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
                break;
            
            case GLFW_KEY_1:
                currentMode = ShadingMode::Flat;
                renderer.setMode(currentMode);
                std::cout << "Modo: Flat Shading" << std::endl;
                break;
            
            case GLFW_KEY_2:
                currentMode = ShadingMode::Gouraud;
                renderer.setMode(currentMode);
                std::cout << "Modo: Gouraud Shading" << std::endl;
                break;
            
            case GLFW_KEY_3:
                currentMode = ShadingMode::Phong;
                renderer.setMode(currentMode);
                std::cout << "Modo: Phong Shading" << std::endl;
                break;
        }
    });
    
    std::cout << "=== CONTROLES ===" << std::endl;
    std::cout << "ESC - Sair" << std::endl;
    std::cout << "1   - Flat Shading" << std::endl;
    std::cout << "2   - Gouraud Shading" << std::endl;
    std::cout << "3   - Phong Shading" << std::endl;
    std::cout << "==================" << std::endl;

    while (!glfwWindowShouldClose(app.window())) {
        app.beginFrame();

        // Redimensionar framebuffer se necessário
        int w, h;
        glfwGetFramebufferSize(app.window(), &w, &h);
        if (w != fb.width() || h != fb.height()) {
            fb.resize(w, h);
        }

        // Limpar buffers CPU
        fb.clear({30, 30, 40, 255}); // fundo cinza escuro
        fb.clearDepth(1000.0f); // valor alto mas razoável
        
        // Animar rotação do cubo
        rotation += 0.01f;
        
        // Aplicar transformação no cubo (rotação)
        Polyhedron transformedCube = cube;
        float cosR = std::cos(rotation);
        float sinR = std::sin(rotation);
        
        for (auto& v : transformedCube.verts) {
            // Rotação em Y
            float x = v.position.x;
            float z = v.position.z;
            v.position.x = x * cosR - z * sinR;
            v.position.z = x * sinR + z * cosR;
            
            // Rotação em X (mais lenta)
            float y = v.position.y;
            z = v.position.z;
            v.position.y = y * std::cos(rotation * 0.5f) - z * std::sin(rotation * 0.5f);
            v.position.z = y * std::sin(rotation * 0.5f) + z * std::cos(rotation * 0.5f);
            
            // Rotacionar normais também
            x = v.normal.x;
            z = v.normal.z;
            v.normal.x = x * cosR - z * sinR;
            v.normal.z = x * sinR + z * cosR;
            
            y = v.normal.y;
            z = v.normal.z;
            v.normal.y = y * std::cos(rotation * 0.5f) - z * std::sin(rotation * 0.5f);
            v.normal.z = y * std::sin(rotation * 0.5f) + z * std::cos(rotation * 0.5f);
            
            v.normal = glm::normalize(v.normal);
        }
        
        // Atualizar posição da câmera no renderer (para Phong)
        renderer.setCameraEye(camera.eye);
        
        // Renderizar cada face do cubo
        for (const auto& face : transformedCube.faces) {
            // Projetar e clipar face
            Polygon poly2D = camera.projectAndClip(transformedCube, face, w, h);
            
            if (poly2D.verts.size() >= 3) {
                // Calcular iluminação baseada no modo de shading
                
                if (currentMode == ShadingMode::Flat) {
                    // FLAT: calcular intensidade uma vez por face usando o centro
                    glm::vec3 faceCenter(0);
                    glm::vec3 faceNormal(0);
                    for (const auto& v : poly2D.verts) {
                        faceCenter += glm::vec3(v.x, v.y, v.z);
                        faceNormal += v.normal;
                    }
                    faceCenter /= float(poly2D.verts.size());
                    faceNormal = glm::normalize(faceNormal);
                    
                    float intensity = renderer.phong(faceCenter, faceNormal, *poly2D.material);
                    renderer.setFlatIntensity(intensity);
                    
                } else if (currentMode == ShadingMode::Gouraud) {
                    // GOURAUD: calcular intensidade por vértice
                    for (auto& v : poly2D.verts) {
                        glm::vec3 pos3D(v.x, v.y, v.z);
                        v.intensity = renderer.phong(pos3D, v.normal, *poly2D.material);
                    }
                }
                // PHONG: intensidade será calculada por pixel no scanline
                
                // Preencher polígono usando scanline (escreve no framebuffer/zbuffer)
                fill_polygon(poly2D, fb, renderer);
            }
        }

        // Agora que tudo foi rasterizado no framebuffer, enviar para OpenGL
        app.drawFramebuffer(fb.colorData(), fb.width(), fb.height());

        app.endFrame();
    }

    app.shutdown();
    return 0;
}
