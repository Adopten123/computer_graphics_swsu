#include "bresenham_algorithm.h"

namespace bresenham {
    using namespace cimg_library;
    using Color = const unsigned char*;

    void draw_circle(CImg<unsigned char>& image, CircleParams params, Color color) {
        int x = 0;
        int y = params.radius;
        int delta = 2 - 2 * params.radius;

        while (x <= y) {
            image.draw_point(params.a + x, params.b + y, color);
            image.draw_point(params.a - x, params.b + y, color);
            image.draw_point(params.a + x, params.b - y, color);
            image.draw_point(params.a - x, params.b - y, color);
            image.draw_point(params.a + y, params.b + x, color);
            image.draw_point(params.a - y, params.b + x, color);
            image.draw_point(params.a + y, params.b - x, color);
            image.draw_point(params.a - y, params.b - x, color);

            int d1 = 2 * (delta + y) - 1;
            int d2 = 2 * (delta - x) - 1;

            if (delta < 0 && d1 <= 0)
                delta += 2 * ++x + 1;
            else if (delta > 0 && d2 > 0)
                delta -= 2 * --y - 1;
            else
                delta += 2 * (++x - --y);
        }
    }
}