#include "cimg_algorithm.h"

namespace cimg_algorithm {
    using namespace cimg_library;
    using Color = const unsigned char*;

    void draw_circle(CImg<unsigned char>& image, CircleParams params, Color color){
        image.draw_circle(params.a, params.b, params.radius, color);
        image.draw_circle(params.a, params.b, params.radius - 1, WHITE_BACK);
    }
}