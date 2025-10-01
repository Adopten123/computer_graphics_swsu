#include "cimg_algorithm.h"

namespace algorithm {
    using namespace cimg_library;
    void draw_line(CImg<unsigned char>& image, LineParams params){
        image.draw_line(params.x1, params.y1, params.x2, params.y2, BLACK);
    }
}