#pragma once

#include "svgprocessor_utils.h"

namespace bresenham {
    using Color = const unsigned char*;
    void draw_circle(cimg_library::CImg<unsigned char>& image, CircleParams params, Color color);
}