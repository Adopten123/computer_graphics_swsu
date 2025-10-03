#include "param_equation_algorithm.h"
#include <cmath>

namespace simple_algorithm {
    using namespace cimg_library;

    using Color = const unsigned char*;

    void draw_circle_params(CImg<unsigned char>& image, CircleParams params, Color color) {
        for (double t = 0; t < 2 * PI; t += 0.001) {
            int x = static_cast<int>(params.a + params.radius * std::cos(t));
            int y = static_cast<int>(params.b + params.radius * std::sin(t));

            if (x >= 0 && x < image.width() && y >= 0 && y < image.height())
                image.draw_point(x, y, color);
        }
    }
}