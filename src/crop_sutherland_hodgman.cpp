#include "../include/crop_sutherland_hodgman.h"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>

struct V2f {
    float x, y, z;  // precisa ser em float para calcular interseções
    glm::vec3 normal;
    float intensity;
};

static V2f toV2f(const Vertex2D& v) {
    V2f r;
    r.x = (float)v.x;
    r.y = (float)v.y;
    r.z = v.z;
    r.normal = v.normal;
    r.intensity = v.intensity;
    return r;
}

static Vertex2D toV2d(const V2f& v) {
    Vertex2D r;
    r.x = (int)std::lround(v.x);
    r.y = (int)std::lround(v.y);
    r.z = v.z;
    r.normal = glm::normalize(v.normal);
    r.intensity = v.intensity;
    return r;
}

// ------------------------------------------------------------
// Interpolação linear dos atributos para novos vértices
// ------------------------------------------------------------
static V2f interp(const V2f& a, const V2f& b, float t) {
    V2f r;
    r.x = a.x + (b.x - a.x) * t;
    r.y = a.y + (b.y - a.y) * t;
    r.z = a.z + (b.z - a.z) * t;
    r.normal = a.normal + (b.normal - a.normal) * t;
    r.intensity = a.intensity + (b.intensity - a.intensity) * t;
    return r;
}

static bool insideLeft(const V2f& p, float xmin) { return p.x >= xmin; }
static bool insideRight(const V2f& p, float xmax) { return p.x <= xmax; }
static bool insideTop(const V2f& p, float ymin) { return p.y >= ymin; }
static bool insideBottom(const V2f& p, float ymax) { return p.y <= ymax; }

static V2f intersectEdge(const V2f& s, const V2f& p, float A, char edge) {
    float dx = p.x - s.x;
    float dy = p.y - s.y;
    float t = 0.0f;

    // evita divisão por zero
    const float eps = 1e-6f;

    switch (edge) {
        case 'L':
            t = (A - s.x) / ((dx != 0) ? dx : eps);
            break;
        case 'R':
            t = (A - s.x) / ((dx != 0) ? dx : eps);
            break;
        case 'T':
            t = (A - s.y) / ((dy != 0) ? dy : eps);
            break;
        case 'B':
            t = (A - s.y) / ((dy != 0) ? dy : eps);
            break;
    }

    t = std::clamp(t, 0.0f, 1.0f);
    return interp(s, p, t);
}

static std::vector<V2f> clipAgainstEdge(const std::vector<V2f>& in, float A,
                                        char edge) {
    std::vector<V2f> out;
    if (in.empty()) return out;

    auto inside = [&](const V2f& p) {
        switch (edge) {
            case 'L':
                return p.x >= A;
            case 'R':
                return p.x <= A;
            case 'T':
                return p.y >= A;
            case 'B':
                return p.y <= A;
        }
        return false;
    };

    V2f S = in.back();
    bool S_in = inside(S);

    for (const V2f& P : in) {
        bool P_in = inside(P);

        if (P_in && S_in) {
            // caso 1: S in → P in
            out.push_back(P);

        } else if (P_in && !S_in) {
            // caso 2: S out → P in
            out.push_back(intersectEdge(S, P, A, edge));
            out.push_back(P);

        } else if (!P_in && S_in) {
            // caso 4: S in → P out
            out.push_back(intersectEdge(S, P, A, edge));

        } else {
            // caso 3: S out → P out
        }

        S = P;
        S_in = P_in;
    }
    return out;
}


Polygon clipPolygon2D(const Polygon& in, int W, int H) {
    Polygon out;
    out.material = in.material;

    if (in.verts.size() < 3) return out;

    float xmin = 0.0f;
    float xmax = (float)(W - 1);
    float ymin = 0.0f;
    float ymax = (float)(H - 1);

    // converte para floats
    std::vector<V2f> poly;
    poly.reserve(in.verts.size());
    for (const auto& v : in.verts) poly.push_back(toV2f(v));

    poly = clipAgainstEdge(poly, xmin, 'L');
    if (poly.empty()) return out;

    poly = clipAgainstEdge(poly, xmax, 'R');
    if (poly.empty()) return out;

    poly = clipAgainstEdge(poly, ymin, 'T');
    if (poly.empty()) return out;

    poly = clipAgainstEdge(poly, ymax, 'B');
    if (poly.empty()) return out;

    // converte de volta para Vertex2D
    out.verts.reserve(poly.size());
    for (const auto& v : poly) out.verts.push_back(toV2d(v));

    return out;
}
