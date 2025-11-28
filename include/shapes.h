#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "types.h"

class Shape {
public:
    Polyhedron mesh;

    Shape(const Polyhedron& p);

    void translate(const glm::vec3& delta);
    void scale(float factor);
    void scale(const glm::vec3& factors);
    void rotateX(float deg);
    void rotateY(float deg);
    void rotateZ(float deg);

private:
    void applyMatrix(const glm::mat4& M);
};

class Shapes {
public:
    std::vector<Shape> objects;

    Shape& createCube(const glm::vec3& center = {0,0,0}, float size = 2.0f);
    Shape& createPyramid(const glm::vec3& center, float size, float height);
    Shape& createCylinder(const glm::vec3& center, float radius, float height, int slices = 16);
    Shape& createSphere(const glm::vec3& center, float radius, int stacks = 12, int slices = 24);
};
