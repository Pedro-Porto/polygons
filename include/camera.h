#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "crop_sutherland_hodgman.h"
#include "types.h"

class Camera {
   public:
    enum class ProjType { Ortho, Perspective };
    enum class MoveType { Orbit, Fps };

    ProjType type = ProjType::Perspective;
    MoveType moveType = MoveType::Orbit;

    // ORTHO
    float orthoHeight = 10.0f;

    // PERSPECTIVE
    float fovY = 60.0f;  // angulo de visão
    float nearp = 0.1f;
    float farp = 100.0f;

    Camera() {}

    bool projectVertex(const glm::vec3& pos, const glm::vec3& normal,
                       Vertex2D& out, int W, int H) const;

    Polygon projectFace(const Polyhedron& obj, const Face& face, int W,
                        int H) const;

    Polygon projectAndClip(const Polyhedron& obj, const Face& face, int W,
                           int H) const;

    void addX(float deltaDeg) {
        if (moveType == MoveType::Orbit)
            addAzimuth(deltaDeg);
        else
            addYaw(deltaDeg);
    }
    void addY(float deltaDeg) {
        if (moveType == MoveType::Orbit)
            addElevation(deltaDeg);
        else
            addPitch(deltaDeg);
    }

    void addOrbitDistance(float delta) {
        orbitDistance += delta;
        if (orbitDistance < 0.1f) orbitDistance = 0.1f;
        updateFromOrbitAngles();
    }

    void moveX(float delta);  // strafe direita/esquerda
    void moveY(float delta);  // sobe/desce
    void moveZ(float delta);  // frente/trás

    glm::vec3 eye = {0, 0, 5};   // local da camera
    glm::vec3 look = {0, 0, 0};  // ponto que a câmera olha (focal point)
    glm::vec3 up = {0, 1, 0};

   private:
    // distância da câmera ao focal point (para modo órbita)
    float orbitDistance = 10.0f;
    // pose da câmera
    float azimuth = 0.0f;    // graus
    float elevation = 0.0f;  // graus

    // FPS MODE (yaw/pitch a partir do eye)
    float yaw = -90.0f;  // graus
    float pitch = 0.0f;  // graus

    void addAzimuth(float deltaDeg);
    void addElevation(float deltaDeg);
    void addYaw(float deltaDeg);
    void addPitch(float deltaDeg);
    void updateFromOrbitAngles();  // usa azimuth/elevation
    void updateFromFPSAngles();    // usa yaw/pitch
};
