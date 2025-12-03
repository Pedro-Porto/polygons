#pragma once
#include <vector>
#include <cstddef>
#include <glm/glm.hpp>

#include "types.h"  // para Line3D, Color

class Lines {
public:
    std::vector<Line3D> objects;

    // adiciona uma nova linha e retorna referência pra ela
    Line3D& add(const glm::vec3& a,
                const glm::vec3& b,
                Color c,
                int w = 1)
    {
        objects.push_back(Line3D{a, b, c, w});
        return objects.back();
    }

    // acesso por índice (para modificar depois)
    Line3D& get(std::size_t i)             { return objects.at(i); }
    const Line3D& get(std::size_t i) const { return objects.at(i); }

    // atalhos
    Line3D& operator[](std::size_t i)             { return objects[i]; }
    const Line3D& operator[](std::size_t i) const { return objects[i]; }

    std::size_t size() const { return objects.size(); }

    void clear() { objects.clear(); }

    // helpers para modificar direto
    void setEndpoints(std::size_t i,
                      const glm::vec3& a,
                      const glm::vec3& b)
    {
        auto& L = objects.at(i);
        L.p1 = a;
        L.p2 = b;
    }

    void setColor(std::size_t i, Color c) {
        objects.at(i).color = c;
    }

    void setWidth(std::size_t i, int w) {
        objects.at(i).width = w;
    }
};
