#pragma once

#include "svgprocessor_utils.h"

namespace bresenham {
    void draw_line_int(cimg_library::CImg<unsigned char>& image, LineParams params);
}