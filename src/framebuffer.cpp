#include "../include/framebuffer.h"

void Framebuffer::resize(int w, int h) {
    W = w; H = h;
    colorBuf.assign(W * H, 0); // preto transparente por padrão
    zBuf.assign(W * H, std::numeric_limits<float>::infinity());
}

void Framebuffer::clear(Color c) {
    uint32_t v = pack(c);
    for (int i = 0; i < W*H; i++) colorBuf[i] = v;
}

void Framebuffer::clearDepth(float z) {
    for (int i = 0; i < W*H; i++) zBuf[i] = z;
}

int Framebuffer::set(int x, int y, float z, Color c) {
    // clip simples na tela
    if (x < 0 || y < 0 || x >= W || y >= H) return 0;

    int i = y*W + x;
    if (z < zBuf[i]) {
        zBuf[i] = z;
        colorBuf[i] = pack(c);
        return 1;
    }
    return 0;
}
