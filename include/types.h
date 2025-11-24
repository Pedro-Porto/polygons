#pragma once
#include <cstdint>
#include <glm/vec3.hpp>
#include <vector>

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

