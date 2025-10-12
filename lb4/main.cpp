#include "svgprocessor_utils.h"
#include "equation_algorithm.h"
#include "param_equation_algorithm.h"
#include "bresenham_algorithm.h"
#include "cimg_algorithm.h"

#include <iostream>
#include <cmath>
#include <string>

using namespace cimg_library;
using Color = const unsigned char*;

void(*FUNCTIONS[4])(CImg<unsigned char>&, CircleParams, Color) = {
    simple_algorithm::draw_circle,
    simple_algorithm::draw_circle_params,
    bresenham::draw_circle,
    cimg_algorithm::draw_circle
};
const char* NAMES[4] = {
    "Equation", "Parametric", "Bresenham", "CImg"
};

void test_circle_algorithms(int width, int height, int radius){
    CImg<unsigned char> test_image(width, height, IMAGE_TYPE, COLOR_CHANNELS_COUNT, WHITE);
    std::vector<std::vector<double>> centers = compute_coords(width, height);

    for(int i = 0; i < 4; i++){
        CircleParams circle_param = CircleParams{
                                    static_cast<int>(centers[i][0]),
                                    static_cast<int>(centers[i][1]),
                                    static_cast<int>(radius)};
        FUNCTIONS[i](test_image, circle_param, BLACK);
        test_image.draw_text(centers[i][0] - 40, centers[i][1] - static_cast<int>(radius) - 30,
                   NAMES[i], BLACK, 0, 1, 13);
    }

    test_image.save_pnm("circle_comparison.pnm", 1);
    test_image.display();
}

void test_pentagon_algorithms(double side_length, int width, int height) {
    CImg<unsigned char> test_image(width, height, IMAGE_TYPE, COLOR_CHANNELS_COUNT, WHITE);

    std::vector<std::vector<double>> centers = compute_coords(width, height);
    for (int i = 0; i < 4; i++) {
        draw_pentagon_with_circles(test_image, centers[i][0], centers[i][1],
                              side_length, FUNCTIONS[i], NAMES[i]);
    }

    test_image.save_pnm("pentagon_comparison.pnm", 3);
    test_image.display();
}

int main(int argc, const char** argv) {
    const std::string code = std::string(argv[1]);

    if (code == "pentagon"){
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <side_length> <width> <height>" << std::endl;
            std::cerr << "Example: " << argv[0] << " 150 800 800" << std::endl;
            return 1;
        }
        const double SIDE_LENGTH = std::stod(argv[2]);
        const int WIDTH = std::stoi(argv[3]);
        const int HEIGHT = std::stoi(argv[4]);
        test_pentagon_algorithms(SIDE_LENGTH, WIDTH, HEIGHT);
    }
    else if (code == "circle"){
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " <width> <height> <radius>" << std::endl;
            std::cerr << "Example: " << argv[0] << " 800 800 150" << std::endl;
            return 2;
        }
        const int WIDTH = std::stoi(argv[2]);
        const int HEIGHT = std::stoi(argv[3]);
        const int RADIUS = std::stoi(argv[4]);
        test_circle_algorithms(WIDTH, HEIGHT, RADIUS);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " <side_length> <width> <height>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 150 800 800" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <width> <height> <radius>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 800 800 150" << std::endl;
        return 3;
    }

    return 0;
}