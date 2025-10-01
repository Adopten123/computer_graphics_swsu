#include "svgprocessor_utils.h"

using namespace std;
using namespace cimg_library;

int sign(double param) {
    return param == 0 ? 0 : param > 0 ? 1 : -1;
}

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

vector<pair<double, double>> calculate_pentagram_vertices(
    double center_x, double center_y, double radius, double start_angle) {

    vector<pair<double, double>> vertices;
    double angle = start_angle * M_PI / 180.0;

    for (int i = 0; i < 5; ++i) {
        double current_angle = angle + i * 2 * M_PI / 5;
        double x = center_x + radius * cos(current_angle);
        double y = center_y + radius * sin(current_angle);
        vertices.push_back({x, y});
    }

    return vertices;
}

void draw_pentagram(
    CImg<unsigned char>& image,
    double center_x, double center_y, double radius, double angle,
    void (*draw_algorithm)(CImg<unsigned char>&, LineParams)) {

    auto vertices = calculate_pentagram_vertices(center_x, center_y, radius, angle);

    int connections[5][2] = {{0, 2}, {2, 4}, {4, 1}, {1, 3}, {3, 0}};

    for (int i = 0; i < 5; ++i) {
        int idx1 = connections[i][0];
        int idx2 = connections[i][1];

        LineParams line{
            static_cast<int>(round(vertices[idx1].first)),
            static_cast<int>(round(vertices[idx1].second)),
            static_cast<int>(round(vertices[idx2].first)),
            static_cast<int>(round(vertices[idx2].second))
        };

        draw_algorithm(image, line);
    }
}