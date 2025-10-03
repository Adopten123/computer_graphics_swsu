#include "equation_algorithm.h"
#include <cmath>

namespace simple_algorithm {
    using namespace cimg_library;

    using Color = const unsigned char*;

    void draw_circle_params_by_axis(CImg<unsigned char>& image, CircleParams params,
                                    Color color, bool is_x_axis){

        int start = is_x_axis ? (params.a - params.radius) : (params.b - params.radius);
        int end = is_x_axis ? (params.a + params.radius) : (params.b + params.radius);

        int delta = is_x_axis ? params.a : params.b;
        int gamma = is_x_axis ? params.b : params.a;
        int image_param_1 = is_x_axis ? image.width() : image.height();
        int image_param_2 = is_x_axis ? image.height() : image.width();

        for (int i = start; i <= end; i++){
            if (i < 0 || i >= image_param_1) continue;

            double di = std::pow(i - delta, 2);
            double squared_radius = std::pow(params.radius, 2);
            if (di > squared_radius) continue;

            double dj = std::sqrt(squared_radius - di);

            int j1 = static_cast<int>(gamma + dj);
            int j2 = static_cast<int>(gamma - dj);

            if (j1 >= 0 && j1 < image_param_2)
                image.draw_point(i, j1, color);
            if (j2 >= 0 && j2 < image_param_2)
                image.draw_point(i, j2, color);
        }
    }

    void draw_circle(CImg<unsigned char>& image, CircleParams params, Color color) {
        draw_circle_params_by_axis(image, params, color, true);
        draw_circle_params_by_axis(image, params, color, false);
    }
}