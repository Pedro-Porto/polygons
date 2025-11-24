#pragma once
#include <vector>
#include <glm/vec3.hpp>

#include "types.h"

// Recorta um polígono 2D contra a viewport [0..W-1] x [0..H-1].
// Retorna polígono novo (pode voltar vazio).
Polygon clipPolygon2D(const Polygon& in, int W, int H);
