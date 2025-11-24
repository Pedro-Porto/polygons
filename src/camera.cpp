#include "../include/camera.h"

static glm::mat3 makeView(const glm::vec3& eye,
                          const glm::vec3& look,
                          const glm::vec3& up)
{
    glm::vec3 f = glm::normalize(look - eye);
    glm::vec3 r = glm::normalize(glm::cross(f, up));
    glm::vec3 u = glm::cross(r, f);

    glm::mat3 V;
    V[0] = r;
    V[1] = u;
    V[2] = -f;
    return V;
}

bool Camera::projectVertex(
    const glm::vec3& pos,
    const glm::vec3& normal,
    Vertex2D& out,
    int W, int H
) const
{
    glm::mat3 V = makeView(eye, look, up);

    // posição relativa à câmera
    glm::vec3 p = V * (pos - eye);

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

        float k = f / z; // fator de perspectiva

        x_ndc = p.x * k / aspect;
        y_ndc = p.y * k;
    }

    // descarta se muito fora da tela
    if (x_ndc < -1.5f || x_ndc > 1.5f ||
        y_ndc < -1.5f || y_ndc > 1.5f)
        return false;

    // converte para pixel
    out.x = (int)((x_ndc * 0.5f + 0.5f) * W);
    out.y = (int)((1.0f - (y_ndc * 0.5f + 0.5f)) * H);

    out.z = -p.z;
    out.normal = normal;
    out.intensity = 1.0f;

    return true;
}

Polygon Camera::projectFace(const Polyhedron& obj,
                            const Face& face,
                            int W, int H) const
{
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

Polygon Camera::projectAndClip(const Polyhedron& obj,
                               const Face& face,
                               int W, int H) const
{
    Polygon poly = projectFace(obj, face, W, H);
    if (poly.verts.size() < 3)
        return Polygon();

    return clipPolygon2D(poly, W, H);
}
