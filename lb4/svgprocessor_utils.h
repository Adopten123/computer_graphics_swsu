#pragma once

#include "CImg.h"
#include <vector>

const int IMAGE_TYPE = 1;
const int COLOR_CHANNELS_COUNT = 3;

const double PI = 3.141592653589793;

const unsigned char BLACK[] = {0, 0, 0};
const unsigned char RED[] = {255, 0, 0};
const unsigned char GREEN[] = {0, 255, 0};
const unsigned char BLUE[] = {0, 0, 255};
const unsigned char WHITE_BACK[] = {255, 255, 255};
const int WHITE = 255;

struct CircleParams {
    int a, b, radius;
};

std::vector<std::vector<double>> compute_coords(int width, int height);

template<typename DrawCircleFunc>
void draw_pentagon_with_circles(cimg_library::CImg<unsigned char>& image,
                               double center_x, double center_y,
                               double side_length,
                               DrawCircleFunc draw_circle_func,
                               const char* algorithm_name) {
    double R = side_length / (2 * sin(M_PI / 5));
    double r = side_length / (2 * tan(M_PI / 5));

    CircleParams outer_circle{static_cast<int>(center_x), static_cast<int>(center_y), static_cast<int>(R)};
    CircleParams inner_circle{static_cast<int>(center_x), static_cast<int>(center_y), static_cast<int>(r)};

    draw_circle_func(image, outer_circle, BLUE);

    draw_circle_func(image, inner_circle, GREEN);

    std::vector<std::pair<int, int>> vertices;
    for (int i = 0; i < 5; i++) {
        double angle = 2 * M_PI * i / 5 - M_PI / 2;
        int x = center_x + static_cast<int>(R * cos(angle));
        int y = center_y + static_cast<int>(R * sin(angle));
        vertices.push_back({x, y});
    }

    for (int i = 0; i < 5; i++) {
        int next = (i + 1) % 5;
        image.draw_line(vertices[i].first, vertices[i].second,
                       vertices[next].first, vertices[next].second, RED);
    }

    image.draw_text(center_x - 40, center_y - static_cast<int>(R) - 30,
                   algorithm_name, BLACK, 0, 1, 13);
}
