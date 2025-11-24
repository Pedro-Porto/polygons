#pragma once
#include <cstdint>
#include <limits>
#include <vector>

#include "types.h"

class Framebuffer {
   public:
    Framebuffer() : W(0), H(0) {}
    Framebuffer(int w, int h) { resize(w, h); }

    void resize(int w, int h);  // realoca buffers
    void clear(Color c);        // limpa cor
    void clearDepth(float z);   // limpa depth

    int width() const { return W; }
    int height() const { return H; }

    // acesso tipo matriz (SEM checagem de bounds)
    uint32_t& color(int x, int y) { return colorBuf[y * W + x]; }
    float& depth(int x, int y) { return zBuf[y * W + x]; }

    // Z-test + escrita (com clip simples)
    // retorna 1 se escreveu, 0 se não
    int set(int x, int y, float z, Color c);

    // ponteiros crus pra upload (textura)
    uint32_t* colorData() { return colorBuf.data(); }
    float* depthData() { return zBuf.data(); }

   private:
    int W, H;
    std::vector<uint32_t> colorBuf;  // RGBA empacotado
    std::vector<float> zBuf;

    uint32_t pack(Color c) {
        return (uint32_t(c.a) << 24) | (uint32_t(c.b) << 16) |
               (uint32_t(c.g) << 8) | (uint32_t(c.r));
    }
};
