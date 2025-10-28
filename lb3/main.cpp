#include "svgprocessor_utils.h"
#include "bresenham_algorithm.h"
#include "bresenham_algorithm_int.h"
#include "cda.h"
#include "cimg_algorithm.h"

#include <iostream>

using namespace cimg_library;

void test_algorithms(double radius, double angle, int width, int height) {

    CImg<unsigned char> test_image(width, height, IMAGE_TYPE, COLOR_CHANNELS_COUNT, WHITE);

    double centers[4][2] = {
        {width * 0.25, height * 0.25},
        {width * 0.75, height * 0.25},
        {width * 0.25, height * 0.75},
        {width * 0.75, height * 0.75}
    };
    draw_pentagram(test_image, centers[0][0], centers[0][1], radius, angle, CDA::draw_line);
    draw_pentagram(test_image, centers[1][0], centers[1][1], radius, angle, bresenham::draw_line);
    draw_pentagram(test_image, centers[2][0], centers[2][1], radius, angle, bresenham::draw_line_int);
    draw_pentagram(test_image, centers[3][0], centers[3][1], radius, angle, algorithm::draw_line);

    test_image.save_pnm_p3("test.pnm");
    test_image.display();
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <radius> <angle>" << std::endl;
        return 1;
    }
    int radius = std::stod(argv[1]);
    int angle = std::stod(argv[2]);
    const int HEIGHT = 800;
    const int WIDTH = 800;

    test_algorithms(radius, angle, WIDTH, HEIGHT);
    return 0;
}