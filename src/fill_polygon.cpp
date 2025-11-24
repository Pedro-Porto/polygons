#include "../include/fill_polygon.h"

#include <list>

using std::list;
using std::vector;

void fill_block(unsigned int y, int x_start, int x_end, Color color, Framebuffer& fb) {
    for (int x = x_start; x < x_end; x++) {
        write_pixel(x, y, color, fb);
    }
}

int floor_frac(const frac& f) {
    // b > 0
    const int t = f.top, b = f.bottom;
    int q = t / b;
    int r = t % b;
    if (r != 0 && t < 0) --q;
    return q;
}

int ceil_frac(const frac& f) {
    // b > 0
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
    // a < b -> negativo
    // a == b -> 0
    // a > b -> positivo
    return a.top * b.bottom - b.top * a.bottom;
}

void order_xmin(list<node>& aet) {
    aet.sort([](const node& a, const node& b) {
        return compare_frac(a.xmin, b.xmin) < 0;
    });
}

void insert_sorted(list<node>& aet, const node& n) {
    auto it = aet.begin();
    while (it != aet.end() && compare_frac(it->xmin, n.xmin) < 0) {
        ++it;
    }
    aet.insert(it, n);
}

void fill_polygon(polygon p, unsigned int screen_height, Framebuffer& fb) { //! lembrar de nao desenhar na direita e topo
    vector<list<node>> et(screen_height);  // edge table
    list<node> aet;                        // active edge table
    int last_y = 0;
    for (auto l : p.walls) {               // criando ET
        if (l.v1.y == l.v2.y) continue;  // linhas horizontais não entram na ET
        node n;
        int y0 = l.v1.y, y1 = l.v2.y;
        int x0 = l.v1.x, x1 = l.v2.x;

        if (y0 > y1) {
            std::swap(y0, y1);
            std::swap(x0, x1);
        }

        n.xmin.top = x0;
        n.xmin.bottom = 1;
        n.ymax = y1;
        n.m_inv.top = (x1 - x0);
        n.m_inv.bottom = (y1 - y0);

        if (n.ymax > last_y) last_y = n.ymax;
        et[y0].push_back(n);
    }


    for (unsigned int y = 0; y < screen_height; y++) {
        if (y == last_y) break;
        // transferir arestas da ET para AET
        for (const auto& a : et[y]) insert_sorted(aet, a);

        // verificar se tem arestas para remover ymax = y
        for (auto it = aet.begin(); it != aet.end();) {
            if (it->ymax == (int)y)
                it = aet.erase(it);
            else
                ++it;
        }

        // desenhar bloco de pixels
        bool paridade = 0;  // par
        int last_x = 0;
        for (auto& a : aet) {
            int curr_x;
            if (paridade) {  // impar
                curr_x = floor_frac(a.xmin);
                fill_block(y, last_x, curr_x - 1, p.color, fb);
            } else {
                curr_x = ceil_frac(a.xmin);
            }
            last_x = curr_x;
            paridade = !paridade;
        }

        // atualiza valores de xmin
        for (auto& a : aet) {
            a.xmin = add_frac(a.xmin, a.m_inv);
        }

        // mantem aet ordenada
        order_xmin(aet);
    }
}
