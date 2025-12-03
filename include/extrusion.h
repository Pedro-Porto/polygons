#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../include/camera.h"
#include "../include/shapes.h"
#include "../include/types.h"

// Modos de edição
enum class EditMode { None, Draw, Extrude };
enum class DrawPlane { XY, XZ, YZ };

// Estado da extrusão
struct ExtrusionState {
    EditMode mode = EditMode::None;
    DrawPlane plane = DrawPlane::XY;
    std::vector<glm::vec3> polygon3D;
    glm::vec3 previewPoint{0};
    bool hasPreview = false;
    float extrudeDepth = 0.0f;
    glm::vec3 extrudeStartPoint{0};
};

// Calcula um ray a partir da posição do mouse
glm::vec3 screenToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight, const Camera& camera);

// Interseção do ray com o plano de desenho
bool intersectDrawPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir, DrawPlane plane, glm::vec3& hitPoint);

// Constrói o sólido extrudado e adiciona aos shapes
void buildExtrudedSolid(const std::vector<glm::vec3>& basePolygon, DrawPlane plane, float depth, Shapes& shapes, const Material& material);

// Cria um poliedro temporário para preview (não adiciona aos shapes)
Polyhedron createExtrudedPolyhedron(const std::vector<glm::vec3>& basePolygon, DrawPlane plane, float depth, const Material& material);

// Atualiza o preview do ponto baseado na posição do mouse
void updateExtrusionPreview(ExtrusionState& state, double mouseX, double mouseY, int screenWidth, int screenHeight, const Camera& camera);

// Calcula a profundidade da extrusão baseada no movimento do mouse
float calculateExtrudeDepth(const ExtrusionState& state, const glm::vec3& currentPoint);

// Retorna o nome do plano atual
const char* getPlaneNameString(DrawPlane plane);
