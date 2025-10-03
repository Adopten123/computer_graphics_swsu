#pragma once

#include "svgprocessor_utils.h"

namespace simple_algorithm {
    using Color = const unsigned char*;
    void draw_circle_params_by_axis(cimg_library::CImg<unsigned char>& img, CircleParams params,
                                    Color color, bool is_x_axis);

    void draw_circle_params(cimg_library::CImg<unsigned char>& img, CircleParams params, Color color);
}