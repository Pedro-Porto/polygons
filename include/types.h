#pragma once
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>

#define COLOR_BLACK {0,0,0,255}
#define COLOR_WHITE {255,255,255,255}
#define COLOR_RED {255,0,0,255}
#define COLOR_GREEN {0,255,0,255}
#define COLOR_BLUE {0,0,255,255}
#define COLOR_ORANGE {255,165,0,255}
#define COLOR_YELLOW {255,255,0,255}
#define COLOR_INDIGO {75,0,130,255}
#define COLOR_CYAN {0,255,255,255}
#define COLOR_PINK {255,51,255,255}
#define COLOR_HUD {220, 220, 220, 255}

#define MATERIAL_RUBBER {COLOR_WHITE, 0.05, 0.5, 0.7, 10.24}
#define MATERIAL_PLASTIC {COLOR_WHITE, 0.00, 0.5, 0.7, 32}
#define MATERIAL_METAL {COLOR_WHITE, 0.2, 0.7, 0.6, 51.2}
#define MATERIAL_STONE {COLOR_WHITE, 0.25, 0.95, 0.3, 11.24}

struct Color {
    uint8_t r, g, b, a;
};

struct Material {
    Color color{255, 255, 255, 255};

    float ka = 0.1f;  // ambiente
    float kd = 0.8f;  // difusa
    float ks = 0.2f;  // especular
    float shininess = 32.0f;
};

struct Face {
    std::vector<int> idx;
};

struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Vertex2D {
    int x, y;
    float z;

    glm::vec3 normal;  // PHONG
    float intensity;   // GOURAUD
};

struct Polyhedron {
    std::vector<Vertex3D> verts;
    std::vector<Face> faces;

    Material material;
};

struct Polygon {
    std::vector<Vertex2D> verts;  // vertices em ordem
    Material *material = nullptr; // ponteiro pra material original
};

struct Line3D {
    glm::vec3 p1;
    glm::vec3 p2;
    Color color;
    int width = 1;
};

struct Line2D {
    int x1, y1;
    int x2, y2;
    float z1, z2;   // profundidade em cada ponta (já no espaço de câmera/projeção)
    Color color;
    int width = 1;
};
enum class ShapeType{Cube, Cylinder, Sphere, Pyramid};
enum class MenuType{Camera, Color, Shape, Materials, Extrusion};
