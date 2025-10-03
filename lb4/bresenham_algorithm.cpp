#include "bresenham_algorithm.h"

namespace bresenham {
    using namespace cimg_library;
    using Color = const unsigned char*;

    void draw_circle(CImg<unsigned char>& image, CircleParams params, Color color) {
        int x = 0;
        int y = params.radius;
        int d = 3 - 2 * params.radius;

        while (x <= y) {
            image.draw_point(params.a + x, params.b + y, color);
            image.draw_point(params.a - x, params.b + y, color);
            image.draw_point(params.a + x, params.b - y, color);
            image.draw_point(params.a - x, params.b - y, color);
            image.draw_point(params.a + y, params.b + x, color);
            image.draw_point(params.a - y, params.b + x, color);
            image.draw_point(params.a + y, params.b - x, color);
            image.draw_point(params.a - y, params.b - x, color);

            if (d < 0)
                d += 4 * x + 6;
            else
                d += 4 * (x - y--) + 10;
            x++;
        }
    }
}