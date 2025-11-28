#include "../include/fill_polygon.h"

#include <list>
#include <algorithm>
#include <iostream>

using std::list;
using std::vector;

// ------------------------- SCANLINE HELPERS -------------------------

int floor_frac(const frac& f) {
    const int t = f.top, b = f.bottom;
    int q = t / b;
    int r = t % b;
    if (r != 0 && t < 0) --q;
    return q;
}

int ceil_frac(const frac& f) {
    const int t = f.top, b = f.bottom;
    int q = t / b;
    int r = t % b;
    if (r != 0 && t > 0) ++q;
    return q;
}

frac add_frac(const frac& a, const frac& b) {
    frac r;
    if (a.bottom == b.bottom) {
        r.top = a.top + b.top;
        r.bottom = a.bottom;
        return r;
    }
    r.top = a.top * b.bottom + b.top * a.bottom;
    r.bottom = a.bottom * b.bottom;
    return r;
}

int compare_frac(const frac& a, const frac& b) {
    return a.top * b.bottom - b.top * a.bottom;
}


struct node_z {
    int ymax;
    frac xmin;
    frac m_inv;

    float z_min;
    float dz_dy;

    float i_min;      // intensidade por vértice (Gouraud)
    float di_dy;

    glm::vec3 n_min;  // normal na aresta (Phong)
    glm::vec3 dn_dy;
};


// ------------------------- FILL POLYGON -------------------------

void fill_polygon(const Polygon& p,
                  Framebuffer& fb,
                  Renderer& renderer)
{
    if (p.verts.size() < 3) return;
    if (!p.material) return;

    int H = fb.height();
    vector<list<node_z>> et(H);
    list<node_z> aet;

    int min_y = H;
    int max_y = 0;

    size_t N = p.verts.size();

    // ------------------------ ET CONSTRUCTION ------------------------
    for (size_t i = 0; i < N; i++) {
        const Vertex2D& v0 = p.verts[i];
        const Vertex2D& v1 = p.verts[(i + 1) % N];

        if (v0.y == v1.y) continue; // ignore horizontals

        node_z nd;

        int y0 = v0.y, y1 = v1.y;
        int x0 = v0.x, x1 = v1.x;
        float z0 = v0.z, z1 = v1.z;

        float I0 = v0.intensity;
        float I1 = v1.intensity;

        glm::vec3 n0 = v0.normal;
        glm::vec3 n1 = v1.normal;

        // sort so y0 < y1
        if (y0 > y1) {
            std::swap(y0, y1);
            std::swap(x0, x1);
            std::swap(z0, z1);
            std::swap(I0, I1);
            std::swap(n0, n1);
        }

        nd.xmin = {x0, 1};
        nd.ymax = y1;
        nd.m_inv = {x1 - x0, y1 - y0};

        nd.z_min = z0;
        nd.dz_dy = (z1 - z0) / float(y1 - y0);

        nd.i_min = I0;
        nd.di_dy = (I1 - I0) / float(y1 - y0);

        nd.n_min = n0;
        nd.dn_dy = (n1 - n0) / float(y1 - y0);

        min_y = std::min(min_y, y0);
        max_y = std::max(max_y, y1);

        if (y0 >= 0 && y0 < H)
            et[y0].push_back(nd);
    }

    min_y = std::max(min_y, 0);
    max_y = std::min(max_y, H - 1);

    // ------------------------ SCANLINE ------------------------
    for (int y = min_y; y <= max_y; y++) {

        // Add edges starting at this scanline
        for (const auto& e : et[y]) {
            auto it = aet.begin();
            while (it != aet.end() && compare_frac(it->xmin, e.xmin) < 0) ++it;
            aet.insert(it, e);
        }

        // Remove edges reaching ymax
        for (auto it = aet.begin(); it != aet.end();) {
            if (it->ymax == y) it = aet.erase(it);
            else ++it;
        }

        if (aet.empty()) continue;

        // --------- PAIR THE INTERSECTIONS (span fill) ---------
        bool inside = false;
        int last_x = 0;
        float last_z = 0.0f;
        float last_I = 0.0f;
        glm::vec3 last_N(0,0,1);

        for (auto& e : aet) {
            int x_curr = inside ? floor_frac(e.xmin) : ceil_frac(e.xmin);

            if (inside) {
                int x0 = last_x, x1 = x_curr;
                if (x0 > x1) std::swap(x0, x1);

                float z0 = last_z, z1 = e.z_min;
                float dz = (x1 != x0) ? (z1 - z0) / float(x1 - x0) : 0.0f;

                float I0 = last_I, I1 = e.i_min;
                float dI = (x1 != x0) ? (I1 - I0) / float(x1 - x0) : 0.0f;

                glm::vec3 n0 = last_N, n1 = e.n_min;
                glm::vec3 dn = (x1 != x0) ? (n1 - n0) / float(x1 - x0) : glm::vec3(0);

                // ---- PIXEL LOOP ----
                float zz = z0;
                float II = I0;
                glm::vec3 nn = n0;

                for (int x = x0; x <= x1; x++) {

                    // posição aproximada em screen-space
                    glm::vec3 pos(x, y, zz);

                    Color c = renderer.shadePixel(
                        *p.material,
                        II,
                        nn,
                        pos
                    );

                    write_pixel_z(x, y, zz, c, fb);

                    zz += dz;
                    II += dI;
                    nn += dn;
                }
            }

            last_x = x_curr;
            last_z = e.z_min;
            last_I = e.i_min;
            last_N = e.n_min;
            inside = !inside;
        }

        // update AET
        for (auto& e : aet) {
            e.xmin = add_frac(e.xmin, e.m_inv);
            e.z_min += e.dz_dy;
            e.i_min += e.di_dy;
            e.n_min += e.dn_dy;
        }

        aet.sort([](const node_z& a, const node_z& b) {
            return compare_frac(a.xmin, b.xmin) < 0;
        });
    }
}
