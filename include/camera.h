#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "types.h"
#include "crop_sutherland_hodgman.h"

class Camera {
public:
    enum class ProjType { Ortho, Perspective };

    // tipo de projeção
    ProjType type = ProjType::Perspective;

    // pose da câmera
    glm::vec3 eye  = {0, 0, 5};
    glm::vec3 look = {0, 0, 0};
    glm::vec3 up   = {0, 1, 0};

    // ORTHO
    float orthoHeight = 10.0f;

    // PERSPECTIVE
    float fovY  = 60.0f;
    float nearp = 0.1f;
    float farp  = 100.0f;

    Camera() {}

    bool projectVertex(const glm::vec3& pos,
                       const glm::vec3& normal,
                       Vertex2D& out,
                       int W, int H) const;

    Polygon projectFace(const Polyhedron& obj,
                        const Face& face,
                        int W, int H) const;

    Polygon projectAndClip(const Polyhedron& obj,
                           const Face& face,
                           int W, int H) const;
};
