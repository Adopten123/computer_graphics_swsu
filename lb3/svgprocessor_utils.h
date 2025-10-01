#pragma once

#include "CImg.h"
#include <cmath>
#include <vector>

const int IMAGE_TYPE = 1;
const int COLOR_CHANNELS_COUNT = 3;

const unsigned char BLACK[] = {0, 0, 0};
const int WHITE = 255;

const double PI = 3.141592653589793;

struct LineParams {
    int x1, y1, x2, y2;

    inline bool is_degeneracy() {
        return x1 == x2 && y1 == y2;
    }
};

int sign(double param);
void swap(int& a, int& b);

std::vector<std::pair<double, double>> calculate_pentagram_vertices(
    double center_x, double center_y, double radius, double start_angle);

void draw_pentagram(
    cimg_library::CImg<unsigned char>& image,
    double center_x, double center_y, double radius, double angle,
    void (*draw_algorithm)(cimg_library::CImg<unsigned char>&, LineParams));