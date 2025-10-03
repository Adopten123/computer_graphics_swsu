#pragma once

#include "CImg.h"

const int IMAGE_TYPE = 1;
const int COLOR_CHANNELS_COUNT = 3;

const double PI = 3.141592653589793;

const unsigned char BLACK[] = {0, 0, 0};
const int WHITE = 255;

struct CircleParams {
    int a, b, radius;
};
