#include "../include/camera.h"

#include <iostream>

static void getCameraBasis(const glm::vec3& eye, const glm::vec3& look,
                           const glm::vec3& up, glm::vec3& forward,
                           glm::vec3& right, glm::vec3& trueUp) {
    forward = glm::normalize(look - eye);             // Z da câmera
    right = glm::normalize(glm::cross(forward, up));  // X da câmera
    trueUp = glm::normalize(
        glm::cross(right, forward));  // Y da câmera ortogonalizado
}

bool Camera::projectVertex(const glm::vec3& pos, const glm::vec3& normal,
                           Vertex2D& out, int W, int H) const {
    glm::mat4 V = glm::lookAt(eye, look, up);
    glm::vec4 p4 = V * glm::vec4(pos, 1);
    glm::vec3 p = glm::vec3(p4);

    // atrás da câmera → descarta
    if (p.z > -nearp) return false;

    float aspect = (H > 0) ? float(W) / float(H) : 1.0f;

    float x_ndc, y_ndc;

    if (type == ProjType::Ortho) {
        float halfH = orthoHeight * 0.5f;
        float halfW = halfH * aspect;

        x_ndc = p.x / halfW;
        y_ndc = p.y / halfH;
    } else  // Perspective
    {
        float f = 1.0f / tan(glm::radians(fovY) * 0.5f);
        float z = -p.z;  // distancia da câmera

        float k = f / z;  // fator de perspectiva

        x_ndc = p.x * k / aspect;
        y_ndc = p.y * k;
    }

    // descarta se muito fora da tela
    if (x_ndc < -1.5f || x_ndc > 1.5f || y_ndc < -1.5f || y_ndc > 1.5f)
        return false;

    // converte para pixel
    out.x = (int)((x_ndc * 0.5f + 0.5f) * W);
    out.y = (int)((1.0f - (y_ndc * 0.5f + 0.5f)) * H);

    out.z = -p.z;
    out.normal = normal;
    out.intensity = 1.0f;

    return true;
}

Polygon Camera::projectFace(const Polyhedron& obj, const Face& face, int W,
                            int H) const {
    Polygon poly;
    poly.material = (Material*)&obj.material;

    for (int id : face.idx) {
        const auto& v = obj.verts[id];
        Vertex2D v2;

        if (!projectVertex(v.position, v.normal, v2, W, H)) {
            poly.verts.clear();
            return poly;
        }

        poly.verts.push_back(v2);
    }

    return poly;
}

Polygon Camera::projectAndClip(const Polyhedron& obj, const Face& face, int W,
                               int H) const {
    Polygon poly = projectFace(obj, face, W, H);
    if (poly.verts.size() < 3) return Polygon();

    return clipPolygon2D(poly, W, H);
}

void Camera::updateFromOrbitAngles() {
    float az = glm::radians(azimuth);
    float el = glm::radians(elevation);

    // garante distância positiva
    float r = orbitDistance;
    if (r <= 0.0f) r = glm::length(eye - look);

    // converte esféricas → cartesianas
    glm::vec3 offset;
    offset.x = r * cos(el) * cos(az);
    offset.y = r * sin(el);
    offset.z = r * cos(el) * sin(az);

    eye = look + offset;

    // recalcula forward / right / up
    glm::vec3 forward = glm::normalize(look - eye);
    glm::vec3 worldUp(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::addAzimuth(float deltaDeg) {
    azimuth += deltaDeg;
    if (azimuth > 180.0f) azimuth -= 360.0f;
    if (azimuth < -180.0f) azimuth += 360.0f;
    updateFromOrbitAngles();
}

void Camera::addElevation(float deltaDeg) {
    elevation += deltaDeg;
    elevation = glm::clamp(elevation, -89.0f, 89.0f);
    updateFromOrbitAngles();
}

void Camera::updateFromFPSAngles() {
    float y = glm::radians(yaw);
    float p = glm::radians(pitch);

    glm::vec3 dir;
    dir.x = cos(p) * cos(y);
    dir.y = sin(p);
    dir.z = cos(p) * sin(y);

    glm::vec3 forward = glm::normalize(dir);
    look = eye + forward;

    glm::vec3 worldUp(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::addYaw(float deltaDeg) {
    yaw += deltaDeg;
    if (yaw > 180.0f) yaw -= 360.0f;
    if (yaw < -180.0f) yaw += 360.0f;
    updateFromFPSAngles();
}

void Camera::addPitch(float deltaDeg) {
    pitch += deltaDeg;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    updateFromFPSAngles();
}

void Camera::moveX(float delta) {
    glm::vec3 forward, right, camUp;
    getCameraBasis(eye, look, up, forward, right, camUp);

    if (moveType == MoveType::Orbit) {
        // pan em órbita: desloca centro e câmera juntos
        eye += right * delta;
        look += right * delta;
    } else {  // FPS
        // anda lateralmente mantendo direção
        eye += right * delta;
        look += right * delta;
    }
}

void Camera::moveY(float delta) {
    glm::vec3 forward, right, camUp;
    getCameraBasis(eye, look, up, forward, right, camUp);

    if (moveType == MoveType::Orbit) {
        eye += camUp * delta;
        look += camUp * delta;
    } else {  // FPS
        eye += camUp * delta;
        look += camUp * delta;
    }
}

void Camera::moveZ(float delta) {
    glm::vec3 forward, right, camUp;
    getCameraBasis(eye, look, up, forward, right, camUp);

    if (moveType == MoveType::Orbit) {
        // pan para frente/trás: move tudo na direção de forward
        eye += forward * delta;
        look += forward * delta;
    } else {  // FPS
        // anda na direção que está olhando
        eye += forward * delta;
        look += forward * delta;
    }
}

static bool clipSegmentZ(glm::vec3& p0, glm::vec3& p1, float nearp,
                         float farp) {
    float zNear = -nearp;
    float zFar = -farp;  // far é mais longe, portanto z mais negativo

    auto intersectAtZ = [&](float zPlane) -> glm::vec3 {
        float t = (zPlane - p0.z) / (p1.z - p0.z);
        return p0 + t * (p1 - p0);
    };

    float z0 = p0.z;
    float z1 = p1.z;

    // 1) Clipa contra near: queremos z <= zNear
    if (z0 > zNear && z1 > zNear) {
        // ambos atrás da near → completamente fora
        return false;
    }
    if (z0 > zNear) {
        p0 = intersectAtZ(zNear);
        z0 = p0.z;
    } else if (z1 > zNear) {
        p1 = intersectAtZ(zNear);
        z1 = p1.z;
    }

    // 2) Clipa contra far: queremos z >= zFar
    z0 = p0.z;
    z1 = p1.z;

    if (z0 < zFar && z1 < zFar) {
        // ambos além do far → completamente fora
        return false;
    }
    if (z0 < zFar) {
        p0 = intersectAtZ(zFar);
    } else if (z1 < zFar) {
        p1 = intersectAtZ(zFar);
    }

    return true;
}
bool Camera::projectLine(const Line3D& l3, Line2D& out, int W, int H) const
{
    glm::mat4 V = glm::lookAt(eye, look, up);
    glm::vec4 a4 = V * glm::vec4(l3.p1, 1.0f);
    glm::vec4 b4 = V * glm::vec4(l3.p2, 1.0f);

    glm::vec3 a = glm::vec3(a4);
    glm::vec3 b = glm::vec3(b4);

    // clipping em near/far no espaço de câmera
    if (!clipSegmentZ(a, b, nearp, farp)) {
        return false;
    }

    auto projectCamPoint = [&](const glm::vec3& p,
                               int& outX,
                               int& outY,
                               float& outZ) -> bool
    {
        // atrás da câmera → descarta
        if (p.z >= -nearp) return false;

        float aspect = (H > 0) ? float(W) / float(H) : 1.0f;

        float x_ndc, y_ndc;

        if (type == ProjType::Ortho)
        {
            float halfH = orthoHeight * 0.5f;
            float halfW = halfH * aspect;

            x_ndc = p.x / halfW;
            y_ndc = p.y / halfH;
        }
        else // Perspective
        {
            float f = 1.0f / tan(glm::radians(fovY) * 0.5f);
            float z = -p.z; // distancia da câmera

            float k = f / z;

            x_ndc = p.x * k / aspect;
            y_ndc = p.y * k;
        }

        outX = int((x_ndc * 0.5f + 0.5f) * W);
        outY = int((1.0f - (y_ndc * 0.5f + 0.5f)) * H);

        outZ = -p.z;
        return true;
    };

    int x0, y0, x1, y1;
    float z0, z1;

    if (!projectCamPoint(a, x0, y0, z0)) return false;
    if (!projectCamPoint(b, x1, y1, z1)) return false;

    out.x1 = x0; out.y1 = y0; out.z1 = z0;
    out.x2 = x1; out.y2 = y1; out.z2 = z1;
    out.color = l3.color;
    out.width = l3.width;

    return true;
}
