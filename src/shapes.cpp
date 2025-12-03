#include "../include/shapes.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


Shape::Shape(const Polyhedron& p) : mesh(p) {}

void Shape::applyMatrix(const glm::mat4& M) {
    for (auto& v : mesh.verts) {
        glm::vec4 p = M * glm::vec4(v.position, 1.0f);
        v.position = glm::vec3(p);

        glm::vec4 n = M * glm::vec4(v.normal, 0.0f);
        v.normal = glm::normalize(glm::vec3(n));
    }
}

void Shape::translate(const glm::vec3& delta) {
    applyMatrix(glm::translate(glm::mat4(1), delta));
}
void Shape::scale(float factor) {
    applyMatrix(glm::scale(glm::mat4(1), glm::vec3(factor)));
}
void Shape::scale(const glm::vec3& f) {
    applyMatrix(glm::scale(glm::mat4(1), f));
}
void Shape::rotateX(float d) {
    applyMatrix(glm::rotate(glm::mat4(1), glm::radians(d), {1, 0, 0}));
}
void Shape::rotateY(float d) {
    applyMatrix(glm::rotate(glm::mat4(1), glm::radians(d), {0, 1, 0}));
}
void Shape::rotateZ(float d) {
    applyMatrix(glm::rotate(glm::mat4(1), glm::radians(d), {0, 0, 1}));
}


static Polyhedron buildCube() {
    Polyhedron cube;

    // Definir vértices do cubo (-1 a 1)
    std::vector<glm::vec3> positions = {
        {-1, -1, -1},  // 0
        {1, -1, -1},   // 1
        {1, 1, -1},    // 2
        {-1, 1, -1},   // 3
        {-1, -1, 1},   // 4
        {1, -1, 1},    // 5
        {1, 1, 1},     // 6
        {-1, 1, 1}     // 7
    };

    // Calcular normais para cada face
    std::vector<glm::vec3> faceNormals = {
        {0, 0, -1},  // frente
        {0, 0, 1},   // trás
        {-1, 0, 0},  // esquerda
        {1, 0, 0},   // direita
        {0, -1, 0},  // baixo
        {0, 1, 0}    // topo
    };

    // Criar vértices com normais
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex3D v;
        v.position = positions[i];
        v.normal =
            glm::vec3(0);  // será calculada pela média das faces adjacentes
        cube.verts.push_back(v);
    }

    // Calcular normais dos vértices (média das faces adjacentes)
    std::vector<std::vector<int>> vertexFaces = {
        {0, 2, 4},  // 0
        {0, 3, 4},  // 1
        {0, 3, 5},  // 2
        {0, 2, 5},  // 3
        {1, 2, 4},  // 4
        {1, 3, 4},  // 5
        {1, 3, 5},  // 6
        {1, 2, 5}   // 7
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
        {{0, 1, 2, 3}},  // frente  (z = -1)
        {{5, 4, 7, 6}},  // trás    (z = +1)
        {{4, 0, 3, 7}},  // esquerda (x = -1)
        {{1, 5, 6, 2}},  // direita  (x = +1)
        {{4, 5, 1, 0}},  // baixo    (y = -1)
        {{3, 2, 6, 7}}   // topo     (y = +1)
    };

    // Material colorido para cada face
    cube.material.color = {100, 150, 255, 255};  // azul claro

    return cube;
}


static Polyhedron buildPyramid() {
    Polyhedron p;

    // base quadrada + topo
    p.verts = {
        {{-1, -1, -1}, {0, 0, 0}},
        {{1, -1, -1}, {0, 0, 0}},
        {{1, -1, 1}, {0, 0, 0}},
        {{-1, -1, 1}, {0, 0, 0}},
        {{0, 1, 0}, {0, 0, 0}}  // topo
    };

    p.faces = {{{0, 1, 2, 3}},  // base
               {{0, 1, 4}},
               {{1, 2, 4}},
               {{2, 3, 4}},
               {{3, 0, 4}}};

    for (auto& v : p.verts) v.normal = glm::normalize(v.position);

    p.material.color = {200, 150, 80, 255};
    return p;
}


static Polyhedron buildCylinder(int slices) {
    Polyhedron p;
    float step = 2 * M_PI / slices;

    // topo e base
    for (int i = 0; i < slices; i++) {
        float a = i * step;
        float x = cos(a), z = sin(a);
        p.verts.push_back(
            {{x, 1, z}, glm::normalize(glm::vec3{x, 0, z})});  // topo
        p.verts.push_back(
            {{x, -1, z}, glm::normalize(glm::vec3{x, 0, z})});  // baixo
    }

    // faces laterais
    for (int i = 0; i < slices; i++) {
        int i2 = (i + 1) % slices;
        p.faces.push_back({{i * 2, i2 * 2, i2 * 2 + 1, i * 2 + 1}});
    }

    p.material.color = {180, 180, 180, 255};
    return p;
}


static Polyhedron buildSphere(int stacks, int slices) {
    Polyhedron p;

    for (int i = 0; i <= stacks; i++) {
        float v = (float)i / stacks;
        float phi = v * M_PI;

        for (int j = 0; j <= slices; j++) {
            float u = (float)j / slices;
            float theta = u * 2 * M_PI;

            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);

            glm::vec3 n = glm::normalize(glm::vec3{x, y, z});
            p.verts.push_back({{x, y, z}, n});
        }
    }

    int w = slices + 1;

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int a = i * w + j;
            int b = a + w;

            p.faces.push_back({{a, b, b + 1, a + 1}});
        }
    }

    p.material.color = {200, 80, 80, 255};
    return p;
}


Shape& Shapes::createCube(const glm::vec3& center, float size) {
    Shape obj(buildCube());
    obj.scale(size * 0.5f);
    obj.translate(center);
    objects.push_back(obj);
    return objects.back();
}

Shape& Shapes::createPyramid(const glm::vec3& center, float size,
                             float height) {
    Shape obj(buildPyramid());
    obj.scale({size * 0.5f, height * 0.5f, size * 0.5f});
    obj.translate(center);
    objects.push_back(obj);
    return objects.back();
}

Shape& Shapes::createCylinder(const glm::vec3& c, float r, float h,
                              int slices) {
    Shape obj(buildCylinder(slices));
    obj.scale({r, h * 0.5f, r});
    obj.translate(c);
    objects.push_back(obj);
    return objects.back();
}

Shape& Shapes::createSphere(const glm::vec3& c, float r, int stacks,
                            int slices) {
    Shape obj(buildSphere(stacks, slices));
    obj.scale(r);
    obj.translate(c);
    objects.push_back(obj);
    return objects.back();
}
