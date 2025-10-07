#pragma once

#include "svgprocessor_utils.h"

namespace cimg_algorithm {
    using Color = const unsigned char*;
    void draw_circle(cimg_library::CImg<unsigned char>& image, CircleParams params, Color color);
}