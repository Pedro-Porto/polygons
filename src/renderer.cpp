#include "../include/renderer.h"
#include <algorithm>

using glm::vec3;
using std::max;

Color Renderer::modulate(Color c, float I) const {
    c.r = uint8_t(std::clamp(int(c.r * I), 0, 255));
    c.g = uint8_t(std::clamp(int(c.g * I), 0, 255));
    c.b = uint8_t(std::clamp(int(c.b * I), 0, 255));
    return c;
}

float Renderer::phong(const vec3& pos,
                      const vec3& normal,
                      const Material& mat) const
{
    vec3 N = glm::normalize(normal);
    vec3 L = glm::normalize(light.pos - pos);
    vec3 V = glm::normalize(eye - pos);
    vec3 H = glm::normalize(L + V);

    float diff = max(glm::dot(N, L), 0.0f);
    float spec = 0.0f;
    if (diff > 0.0f)
        spec = pow(max(glm::dot(N, H), 0.0f), mat.shininess);

    float I =
        mat.ka +
        mat.kd * diff * light.intensity +
        mat.ks * spec * light.intensity;

    return std::clamp(I, 0.0f, 1.0f);
}

Color Renderer::shadePixel(const Material& mat,
                           float intensity,
                           const vec3& normal,
                           const vec3& pos) const
{
    switch (mode) {
    case ShadingMode::Flat:
        return modulate(mat.color, flatI);

    case ShadingMode::Gouraud:
        return modulate(mat.color, intensity);

    case ShadingMode::Phong: {
        float I = phong(pos, normal, mat);
        return modulate(mat.color, I);
    }
    }

    return mat.color; // fallback
}
