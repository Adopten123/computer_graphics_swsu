#include "equation_algorithm.h"
#include "param_equation_algorithm.h"
#include "bresenham_algorithm.h"

#include "svgprocessor_utils.h"


using namespace cimg_library;

int main(int argc, const char **argv) {
    cimg_library::CImg<unsigned char> test_image(400,400, IMAGE_TYPE, COLOR_CHANNELS_COUNT, WHITE);
    CircleParams params {200,200,100};

    bresenham::draw_circle(test_image, params, BLACK);

    test_image.save_pnm("test.pbm");
    test_image.display();
    return 0;
}