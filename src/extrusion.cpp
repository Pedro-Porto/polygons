#include "../include/extrusion.h"

#include <cmath>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 screenToWorldRay(double mouseX, double mouseY, int screenWidth,
                           int screenHeight, const Camera& camera) {
    float x_ndc = (float(mouseX) / float(screenWidth)) * 2.0f - 1.0f;
    float y_ndc = (float(mouseY) / float(screenHeight)) * 2.0f - 1.0f;
    float aspect = float(screenWidth) / float(screenHeight);
    float f = 1.0f / std::tan(glm::radians(camera.fovY) * 0.5f);
    glm::vec3 rayCam;
    rayCam.x = x_ndc * aspect / f;
    rayCam.y = y_ndc / f;
    rayCam.z = -1.0f;
    glm::mat4 V = glm::lookAt(camera.eye, camera.look, camera.up);
    glm::mat3 inv = glm::inverse(glm::mat3(V));
    return glm::normalize(inv * rayCam);
}

bool intersectDrawPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                        DrawPlane plane, glm::vec3& hitPoint) {
    float t = 0.0f;

    if (plane == DrawPlane::XY) {
        // Plano z = 0
        if (std::fabs(rayDir.z) < 1e-5f) return false;
        t = -rayOrigin.z / rayDir.z;
    } else if (plane == DrawPlane::XZ) {
        // Plano y = 0
        if (std::fabs(rayDir.y) < 1e-5f) return false;
        t = -rayOrigin.y / rayDir.y;
    } else {  // YZ
        // Plano x = 0
        if (std::fabs(rayDir.x) < 1e-5f) return false;
        t = -rayOrigin.x / rayDir.x;
    }

    if (t < 0.0f) return false;  // Ponto está atrás da câmera

    hitPoint = rayOrigin + t * rayDir;
    return true;
}

Polyhedron createExtrudedPolyhedron(const std::vector<glm::vec3>& basePolygon,
                                    DrawPlane plane, float depth,
                                    const Material& material) {
    Polyhedron poly;
    if (basePolygon.size() < 3) return poly;

    int numVertices = (int)basePolygon.size();

    // Determina a direção da extrusão baseada no plano
    glm::vec3 extrudeDirection;
    if (plane == DrawPlane::XY) {
        extrudeDirection = glm::vec3(0, 0, 1);
    } else if (plane == DrawPlane::XZ) {
        extrudeDirection = glm::vec3(0, 1, 0);
    } else {  // YZ
        extrudeDirection = glm::vec3(1, 0, 0);
    }

    // Cria os vértices do topo
    std::vector<glm::vec3> topPolygon = basePolygon;
    for (auto& vertex : topPolygon) {
        vertex += extrudeDirection * depth;
    }

    // Configura o material
    poly.material = material;

    // Aloca vértices (base + topo)
    poly.verts.resize(2 * numVertices);
    for (auto& vert : poly.verts) {
        vert.normal = glm::vec3(0.0f);
    }

    // Preenche vértices da base (índices 0 a N-1)
    for (int i = 0; i < numVertices; ++i) {
        poly.verts[i].position = basePolygon[i];
    }

    // Preenche vértices do topo (índices N a 2N-1)
    for (int i = 0; i < numVertices; ++i) {
        poly.verts[numVertices + i].position = topPolygon[i];
    }

    // Função auxiliar para adicionar face
    auto addFace = [&](const std::vector<int>& indices) {
        Face f;
        f.idx = indices;
        poly.faces.push_back(f);
    };

    // Face da base (mantém a ordem original)
    {
        std::vector<int> indices;
        indices.reserve(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            indices.push_back(i);
        }
        addFace(indices);
    }

    // Face do topo (inverte a ordem para normal apontar para fora)
    {
        std::vector<int> indices;
        indices.reserve(numVertices);
        for (int i = 0; i < numVertices; ++i) {
            indices.push_back(numVertices + (numVertices - 1 - i));
        }
        addFace(indices);
    }

    // Faces laterais (quads)
    for (int i = 0; i < numVertices; ++i) {
        int nextI = (i + 1) % numVertices;
        int baseIdx0 = i;
        int baseIdx1 = nextI;
        int topIdx0 = numVertices + i;
        int topIdx1 = numVertices + nextI;

        // Cria quad
        addFace({baseIdx0, baseIdx1, topIdx1, topIdx0});
    }

    // Calcula normais por face e acumula nos vértices
    for (const auto& face : poly.faces) {
        if (face.idx.size() < 3) continue;

        glm::vec3 p0 = poly.verts[face.idx[0]].position;
        glm::vec3 p1 = poly.verts[face.idx[1]].position;
        glm::vec3 p2 = poly.verts[face.idx[2]].position;

        glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));

        // Acumula normal em todos os vértices da face
        for (int vertexIdx : face.idx) {
            poly.verts[vertexIdx].normal += normal;
        }
    }

    // Normaliza as normais acumuladas
    for (auto& vert : poly.verts) {
        float length = glm::length(vert.normal);
        if (length > 1e-6f) {
            vert.normal /= length;
        } else {
            vert.normal = glm::vec3(0, 1, 0);  // Normal padrão
        }
    }

    return poly;
}

void buildExtrudedSolid(const std::vector<glm::vec3>& basePolygon,
                        DrawPlane plane, float depth, Shapes& shapes,
                        const Material& material) {
    Polyhedron poly =
        createExtrudedPolyhedron(basePolygon, plane, depth, material);
    if (poly.verts.empty()) return;

    shapes.objects.emplace_back(poly);
}

void updateExtrusionPreview(ExtrusionState& state, double mouseX, double mouseY,
                            int screenWidth, int screenHeight,
                            const Camera& camera) {
    glm::vec3 ray =
        screenToWorldRay(mouseX, mouseY, screenWidth, screenHeight, camera);

    if (state.mode == EditMode::Draw) {
        // No modo desenho, intersecta com o plano de desenho
        glm::vec3 hitPoint;
        if (intersectDrawPlane(camera.eye, ray, state.plane, hitPoint)) {
            state.previewPoint = hitPoint;
            state.hasPreview = true;
        } else {
            state.hasPreview = false;
        }
    } else if (state.mode == EditMode::Extrude && state.polygon3D.size() >= 3) {
        // No modo extrusão, queremos pegar um plano paralelo ao plano original
        // e ver onde o ray intersecta para calcular a profundidade
        state.hasPreview = true;

        // Direção da extrusão (perpendicular ao plano)
        glm::vec3 extrudeDir;
        if (state.plane == DrawPlane::XY) {
            extrudeDir = glm::vec3(0, 0, 1);
        } else if (state.plane == DrawPlane::XZ) {
            extrudeDir = glm::vec3(0, 1, 0);
        } else {  // YZ
            extrudeDir = glm::vec3(1, 0, 0);
        }

        // Intersecta o ray com um plano perpendicular ao plano de desenho
        // que passa pelo ponto de referência da câmera
        glm::vec3 planePoint =
            camera.look;  // Usa o ponto de look como referência
        glm::vec3 planeNormal;

        // Normal do plano de interseção (paralelo ao plano de desenho)
        if (state.plane == DrawPlane::XY) {
            planeNormal = glm::vec3(1, 0, 0);  // Plano perpendicular em X
        } else if (state.plane == DrawPlane::XZ) {
            planeNormal = glm::vec3(0, 0, 1);  // Plano perpendicular em Z
        } else {                               // YZ
            planeNormal = glm::vec3(0, 1, 0);  // Plano perpendicular em Y
        }

        // Intersecção ray-plano: t = dot(planePoint - rayOrigin, planeNormal) /
        // dot(rayDir, planeNormal)
        float denom = glm::dot(ray, planeNormal);
        if (std::abs(denom) > 0.001f) {
            float t = glm::dot(planePoint - camera.eye, planeNormal) / denom;
            if (t > 0) {
                glm::vec3 hitPoint = camera.eye + ray * t;

                // Projeta o hit point na direção de extrusão para obter a
                // profundidade
                float depth;
                if (state.plane == DrawPlane::XY) {
                    depth = hitPoint.z - state.extrudeStartPoint.z;
                } else if (state.plane == DrawPlane::XZ) {
                    depth = hitPoint.y - state.extrudeStartPoint.y;
                } else {  // YZ
                    depth = hitPoint.x - state.extrudeStartPoint.x;
                }

                state.extrudeDepth = depth;
            }
        }
    }
}

float calculateExtrudeDepth(const ExtrusionState& state,
                            const glm::vec3& currentPoint) {
    if (state.polygon3D.empty()) return 0.0f;

    // Calcula a profundidade baseada no plano e no ponto de partida
    float depth = 0.0f;

    if (state.plane == DrawPlane::XY) {
        depth = currentPoint.z - state.extrudeStartPoint.z;
    } else if (state.plane == DrawPlane::XZ) {
        depth = currentPoint.y - state.extrudeStartPoint.y;
    } else {  // YZ
        depth = currentPoint.x - state.extrudeStartPoint.x;
    }

    return depth;
}

const char* getPlaneNameString(DrawPlane plane) {
    switch (plane) {
        case DrawPlane::XY:
            return "XY";
        case DrawPlane::XZ:
            return "XZ";
        case DrawPlane::YZ:
            return "YZ";
        default:
            return "??";
    }
}
