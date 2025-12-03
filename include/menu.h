#pragma once

#include <string>
#include "../include/types.h"
#include "../include/draw.h"
#include "../include/font8x8.h"
#include "../include/framebuffer.h"
#include "../include/camera.h"
#include "../include/renderer.h"
#include "../include/extrusion.h"

bool color_check(Color c1, Color c2){
    if(c1.a==c2.a && c1.r==c2.r && c1.g==c2.g && c1.b==c2.b ) return true;
    return false;
}

bool coefs_check(Material m1, Material m2){
    if(m1.ka==m2.ka && m1.kd==m2.kd && m1.ks==m2.ks && m1.shininess==m2.shininess ) return true;
    return false;
}

void camera_menu(Framebuffer& fb, Camera camera, ShadingMode currentMode, int fps, int fontScale, int lineH)
{
    drawText(fb, 10, 10, "=== MENU PRINCIPAL ===", COLOR_HUD, fontScale);
    
    drawText(
        fb, 10, 10 + lineH,
        std::string("PROJ: ") +
            (camera.type == Camera::ProjType::Perspective ? "PERSPECTIVE" : "ORTHO") + " (7)",
        COLOR_HUD, fontScale);

    drawText(fb, 10, 10 + 2 * lineH,
             std::string("SHADING: ") +
                 (currentMode == ShadingMode::Flat      ? "FLAT"
                  : currentMode == ShadingMode::Gouraud ? "GOURAUD"
                                                        : "PHONG") + " (1/2/3)",
             COLOR_HUD, fontScale);

    drawText(
        fb, 10, 10 + 3 * lineH,
        std::string("CAMERA: ") +
            (camera.moveType == Camera::MoveType::Orbit ? "ORBIT" : "FPS") + " (9)",
        COLOR_HUD, fontScale);

    drawText(fb, 10, 10 + 4 * lineH,
             std::string("FPS: ") + std::to_string(fps), COLOR_HUD,
             fontScale);
    
    drawText(fb, 10, 10 + 5 * lineH, "C: COR   F: FORMA   M: MATERIAL", COLOR_HUD, fontScale);
    drawText(fb, 10, 10 + 6 * lineH, "G: EXTRUDE   ESC: SAIR", COLOR_HUD, fontScale);
}

void color_menu(Framebuffer& fb, Material& material, int fontScale, int lineH){
    drawText(fb, 10, 10, "=== ESCOLHA COR ===", COLOR_HUD, fontScale);
    
    drawText(fb, 10, 10 + 1*lineH, "ESC: VOLTAR", COLOR_HUD, fontScale);
    
    drawText(
        fb, 10, 10 + 2*lineH,
        std::string("1: PRETO ") +
        ( color_check(material.color, COLOR_BLACK) ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 3*lineH,
        std::string("2: BRANCO ") +
        ( color_check(material.color, COLOR_WHITE) ? "<" : ""),
        COLOR_WHITE, fontScale);
    drawText(
        fb, 10, 10 + 4*lineH,
        std::string("3: VERMELHO ") +
        ( color_check(material.color, COLOR_RED) ? "<" : ""),
        COLOR_RED, fontScale);
    drawText(
        fb, 10, 10 + 5*lineH,
        std::string("4: VERDE ") +
        ( color_check(material.color, COLOR_GREEN) ? "<" : ""),
        COLOR_GREEN, fontScale);
    drawText(
        fb, 10, 10 + 6*lineH,
        std::string("5: AZUL ") +
        ( color_check(material.color, COLOR_BLUE) ? "<" : ""),
        COLOR_BLUE, fontScale);
    drawText(
        fb, 10, 10 + 7*lineH,
        std::string("6: LARANJA ") +
        ( color_check(material.color, COLOR_ORANGE) ? "<" : ""),
        COLOR_ORANGE, fontScale);
    drawText(
        fb, 10, 10 + 8*lineH,
        std::string("7: AMARELO ") +
        ( color_check(material.color, COLOR_YELLOW) ? "<" : ""),
        COLOR_YELLOW, fontScale);
    drawText(
        fb, 10, 10 + 9*lineH,
        std::string("8: INDIGO ") +
        ( color_check(material.color, COLOR_INDIGO) ? "<" : ""),
        COLOR_INDIGO, fontScale);
    drawText(
        fb, 10, 10 + 10*lineH,
        std::string("9: CIANO ") +
        ( color_check(material.color, COLOR_CYAN) ? "<" : ""),
        COLOR_CYAN, fontScale);
    drawText(
        fb, 10, 10 + 11*lineH,
        std::string("0: ROSA ") +
        ( color_check(material.color, COLOR_PINK) ? "<" : ""),
        COLOR_PINK, fontScale);
}

void shape_menu(Framebuffer& fb, ShapeType shape_type, int fontScale, int lineH){
    drawText(fb, 10, 10, "=== ESCOLHA FORMA ===", COLOR_HUD, fontScale);
    
    drawText(fb, 10, 10 + 1*lineH, "ESC: VOLTAR   LMB: CRIAR", COLOR_HUD, fontScale);
    
    drawText(
        fb, 10, 10 + 2*lineH,
        std::string("1: CUBO ") +
        ( shape_type == ShapeType::Cube ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 3*lineH,
        std::string("2: ESFERA ") +
        ( shape_type == ShapeType::Sphere ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 4*lineH,
        std::string("3: CILINDRO ") +
        ( shape_type == ShapeType::Cylinder ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 5*lineH,
        std::string("4: PIRAMIDE ") +
        ( shape_type == ShapeType::Pyramid ? "<" : ""),
        COLOR_HUD, fontScale);
}

void materials_menu(Framebuffer& fb, Material material, int fontScale, int lineH){
    drawText(fb, 10, 10, "=== ESCOLHA MATERIAL ===", COLOR_HUD, fontScale);
    
    drawText(fb, 10, 10 + 1*lineH, "ESC: VOLTAR", COLOR_HUD, fontScale);
    
    drawText(
        fb, 10, 10 + 2*lineH,
        std::string("1: BORRACHA ") +
        ( coefs_check(material, MATERIAL_RUBBER) ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 3*lineH,
        std::string("2: PLASTICO ") +
        ( coefs_check(material, MATERIAL_PLASTIC) ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 4*lineH,
        std::string("3: METAL ") +
        ( coefs_check(material, MATERIAL_METAL) ? "<" : ""),
        COLOR_HUD, fontScale);
    drawText(
        fb, 10, 10 + 5*lineH,
        std::string("4: PEDRA ") +
        ( coefs_check(material, MATERIAL_STONE) ? "<" : ""),
        COLOR_HUD, fontScale);
}

void extrusion_menu(Framebuffer& fb, const ExtrusionState& extrusionState, int fontScale, int lineH){
    drawText(fb, 10, 10, "=== MODO EXTRUDE ===", COLOR_HUD, fontScale);
    
    if (extrusionState.mode == EditMode::None) {
        drawText(fb, 10, 10 + lineH, "O: DESENHAR POLIGONO", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 2*lineH, "P: TROCAR PLANO", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 3*lineH, "ESC: VOLTAR", COLOR_HUD, fontScale);
        
        std::string planeStr = std::string("PLANO: ") + getPlaneNameString(extrusionState.plane);
        drawText(fb, 10, 10 + 5*lineH, planeStr, COLOR_HUD, fontScale);
        
    } else if (extrusionState.mode == EditMode::Draw) {
        drawText(fb, 10, 10 + lineH, "LMB: ADICIONAR VERTICE", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 2*lineH, "SPACE: INICIAR EXTRUDE", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 3*lineH, "ESC: CANCELAR", COLOR_HUD, fontScale);

        std::string info = std::string("PLANO: ") +
                           getPlaneNameString(extrusionState.plane) +
                           "  VERT: " +
                           std::to_string(extrusionState.polygon3D.size());

        drawText(fb, 10, 10 + 5*lineH, info, COLOR_HUD, fontScale);
        
    } else if (extrusionState.mode == EditMode::Extrude) {
        drawText(fb, 10, 10 + lineH, "MOUSE: AJUSTAR ALTURA", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 2*lineH, "SPACE: CONFIRMAR", COLOR_HUD, fontScale);
        drawText(fb, 10, 10 + 3*lineH, "ESC: CANCELAR", COLOR_HUD, fontScale);

        char depthStr[64];
        std::snprintf(depthStr, sizeof(depthStr), "ALTURA: %.2f",
                      extrusionState.extrudeDepth);
        drawText(fb, 10, 10 + 5*lineH, depthStr, COLOR_HUD, fontScale);
    }
}

void menu(MenuType menu_type, ShapeType shape_type, Framebuffer& fb, Camera camera, ShadingMode currentMode, Material material, int fps, const ExtrusionState& extrusionState){

    int fontScale = 2;
    int lineH = 8 * fontScale + 4; // altura da linha com margin

    switch (menu_type)
    {
        case MenuType::Camera:
        camera_menu(fb, camera, currentMode, fps, fontScale, lineH);
        break;
        case MenuType::Color:
        color_menu(fb, material, fontScale, lineH);
        break;
        case MenuType::Shape:
        shape_menu(fb, shape_type, fontScale, lineH);
        break;
        case MenuType::Materials:
        materials_menu(fb, material, fontScale, lineH);
        break;
        case MenuType::Extrusion:
        extrusion_menu(fb, extrusionState, fontScale, lineH);
        break;
    }
}

void extrusion_key(int key, MenuType &menu_type, ShapeType &shape_type, Camera &camera, Material &material, ShadingMode &currentMode, Renderer &renderer, float moveStep, ExtrusionState &extrusionState, Shapes &shapes)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (extrusionState.mode != EditMode::None) {
            extrusionState.mode = EditMode::None;
            extrusionState.polygon3D.clear();
            extrusionState.hasPreview = false;
        } else {
            menu_type = MenuType::Camera;
        }
        break;
    case GLFW_KEY_O:
        extrusionState.mode = EditMode::Draw;
        extrusionState.polygon3D.clear();
        extrusionState.hasPreview = false;
        break;
    case GLFW_KEY_P:
        if (extrusionState.plane == DrawPlane::XY)
            extrusionState.plane = DrawPlane::XZ;
        else if (extrusionState.plane == DrawPlane::XZ)
            extrusionState.plane = DrawPlane::YZ;
        else
            extrusionState.plane = DrawPlane::XY;
        std::cout << "Plano: " << getPlaneNameString(extrusionState.plane) << "\n";
        break;
    case GLFW_KEY_SPACE:
        if (extrusionState.mode == EditMode::Draw &&
            extrusionState.polygon3D.size() >= 3) {
            extrusionState.mode = EditMode::Extrude;
            extrusionState.extrudeDepth = 0.0f;
            extrusionState.extrudeStartPoint = extrusionState.polygon3D[0];
        } else if (extrusionState.mode == EditMode::Extrude) {
            buildExtrudedSolid(extrusionState.polygon3D, extrusionState.plane,
                             extrusionState.extrudeDepth, shapes, material);
            extrusionState.mode = EditMode::None;
            extrusionState.polygon3D.clear();
            extrusionState.hasPreview = false;
        }
        break;
    }
}

void camera_key(int key, MenuType &menu_type, ShapeType &shape_type, Camera &camera, Material &material, ShadingMode &currentMode, Renderer &renderer, float moveStep)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
        break;
    case GLFW_KEY_C:
        menu_type = MenuType::Color;
        break;
    case GLFW_KEY_F:
        menu_type = MenuType::Shape;
        break;
    case GLFW_KEY_M:
        menu_type = MenuType::Materials;
        break;
    case GLFW_KEY_G:
        menu_type = MenuType::Extrusion;
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
        // Toggle entre Perspective e Ortho
        if (camera.type == Camera::ProjType::Perspective) {
            camera.type = Camera::ProjType::Ortho;
            std::cout << "Camera Projection: Orthogonal\n";
        } else {
            camera.type = Camera::ProjType::Perspective;
            std::cout << "Camera Projection: Perspective\n";
        }
        break;
    case GLFW_KEY_9:
        // Toggle entre FPS e Orbit
        if (camera.moveType == Camera::MoveType::Fps) {
            camera.moveType = Camera::MoveType::Orbit;
            std::cout << "Camera Mode: Orbit\n";
        } else {
            camera.moveType = Camera::MoveType::Fps;
            std::cout << "Camera Mode: FPS\n";
        }
        break;
    }
}

void color_key(int key, MenuType &menu_type, ShapeType &shape_type, Camera &camera, Material &material, ShadingMode &currentMode, Renderer &renderer, float moveStep)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        menu_type = MenuType::Camera;
        break;
    case GLFW_KEY_C:
        menu_type = MenuType::Color;
        break;
    case GLFW_KEY_F:
        menu_type = MenuType::Shape;
        break;
    case GLFW_KEY_M:
        menu_type = MenuType::Materials;
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
    case GLFW_KEY_1:
        material.color = COLOR_BLACK;
        break;
    case GLFW_KEY_2:
        material.color = COLOR_WHITE;
        break;
    case GLFW_KEY_3:
        material.color = COLOR_RED;
        break;
    case GLFW_KEY_4:
        material.color = COLOR_GREEN;
        break;
    case GLFW_KEY_5:
        material.color = COLOR_BLUE;
        break;
    case GLFW_KEY_6:
        material.color = COLOR_ORANGE;
        break;
    case GLFW_KEY_7:
        material.color = COLOR_YELLOW;
        break;
    case GLFW_KEY_8:
        material.color = COLOR_INDIGO;
        break;
    case GLFW_KEY_9:
        material.color = COLOR_CYAN;
        break;
    case GLFW_KEY_0:
        material.color = COLOR_PINK;
        break;
    }
}

void shape_key(int key, MenuType &menu_type, ShapeType &shape_type, Camera &camera, Material &material, ShadingMode &currentMode, Renderer &renderer, float moveStep)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        menu_type = MenuType::Camera;
        break;
    case GLFW_KEY_C:
        menu_type = MenuType::Color;
        break;
    case GLFW_KEY_F:
        menu_type = MenuType::Shape;
        break;
    case GLFW_KEY_M:
        menu_type = MenuType::Materials;
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
    case GLFW_KEY_1:
        shape_type = ShapeType::Cube;
        break;
    case GLFW_KEY_2:
        shape_type = ShapeType::Sphere;
        break;
    case GLFW_KEY_3:
        shape_type = ShapeType::Cylinder;
        break;
    case GLFW_KEY_4:
        shape_type = ShapeType::Pyramid;
        break;
    }
}

void materials_key(int key, MenuType &menu_type, ShapeType &shape_type, Camera &camera, Material &material, ShadingMode &currentMode, Renderer &renderer, float moveStep)
{
    Material Rubber = MATERIAL_RUBBER, Plastic = MATERIAL_PLASTIC, Metal = MATERIAL_METAL, Stone = MATERIAL_STONE;
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        menu_type = MenuType::Camera;
        break;
    case GLFW_KEY_C:
        menu_type = MenuType::Color;
        break;
    case GLFW_KEY_F:
        menu_type = MenuType::Shape;
        break;
    case GLFW_KEY_M:
        menu_type = MenuType::Materials;
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
    case GLFW_KEY_1:
        material.ka = Rubber.ka;
        material.kd = Rubber.kd;
        material.ks = Rubber.ks;
        material.shininess = Rubber.shininess;
        break;
    case GLFW_KEY_2:
        material.ka = Plastic.ka;
        material.kd = Plastic.kd;
        material.ks = Plastic.ks;
        material.shininess = Plastic.shininess;
        break;
    case GLFW_KEY_3:
        material.ka = Metal.ka;
        material.kd = Metal.kd;
        material.ks = Metal.ks;
        material.shininess = Metal.shininess;
        break;
    case GLFW_KEY_4:
        material.ka = Stone.ka;
        material.kd = Stone.kd;
        material.ks = Stone.ks;
        material.shininess = Stone.shininess;
        break;
    }
}

void key_press(int key, MenuType& menu_type, ShapeType& shape_type, Camera& camera, Material& material, ShadingMode& currentMode, Renderer& renderer, ExtrusionState& extrusionState, Shapes& shapes){
    const float moveStep = 0.2f;

    switch (menu_type)
    {
    case MenuType::Camera:
        camera_key(key,menu_type,shape_type,camera,material,currentMode,renderer,moveStep);
        break;
    case MenuType::Color:
        color_key(key,menu_type,shape_type,camera,material,currentMode,renderer,moveStep);
        break;
    case MenuType::Shape:
        shape_key(key,menu_type,shape_type,camera,material,currentMode,renderer,moveStep);
        break;
    case MenuType::Materials:
        materials_key(key,menu_type,shape_type,camera,material,currentMode,renderer,moveStep);
        break;
    case MenuType::Extrusion:
        extrusion_key(key,menu_type,shape_type,camera,material,currentMode,renderer,moveStep,extrusionState,shapes);
        break;
    }

}