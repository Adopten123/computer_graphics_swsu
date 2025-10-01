#pragma once

#include "svgprocessor_utils.h"

namespace CDA {

    int compute_l(LineParams params);
    double compute_delta(int fst, int snd, int L);

    void draw_line(cimg_library::CImg<unsigned char>& image, LineParams params);

}