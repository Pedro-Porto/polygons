#ifndef DRAW_H
#define DRAW_H

#include <math.h>
#include <stdio.h>
#include <iostream>
#include "../include/types.h"
#define MAX_WIDTH 10

using namespace std;

void write_pixel(int x, int y, Color color);

void write_pixel(int x, int y, Color color, vector<float>& _gVerts);

void write_pixel_dilated(int x, int y, Color color, int width);


#endif