#pragma once
#include <glm/glm.hpp>
#include "../include/types.h"

enum class ShadingMode {
    Flat,
    Gouraud,
    Phong
};

struct Light {
    glm::vec3 pos   = {0,0,5};
    glm::vec3 color = {1,1,1};
    float intensity = 1.0f;
};

class Renderer {
public:
    Renderer() = default;

    void setLight(const Light& l) { light = l; }
    void setCameraEye(const glm::vec3& e) { eye = e; }
    void setMode(ShadingMode m) { mode = m; }

    // Flat shading precisa ser pré-calculado
    void setFlatIntensity(float I) { flatI = I; }

    // Calcular intensidade Phong por pixel
    float phong(const glm::vec3& pos,
                const glm::vec3& normal,
                const Material& mat) const;

    // usado pelo scanline
    Color shadePixel(const Material& mat,
                     float intensity,         // Gouraud OU Flat
                     const glm::vec3& normal, // para Phong
                     const glm::vec3& pos) const;

private:
    ShadingMode mode = ShadingMode::Flat;
    Light light;
    glm::vec3 eye = {0,0,5};

    float flatI = 1.0f; // intensidade FLAT pré-computada

    Color modulate(Color c, float I) const;
};
