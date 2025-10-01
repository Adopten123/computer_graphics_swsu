#include "bresenham_algorithm.h"

namespace bresenham {
    using namespace cimg_library;

    void draw_line(CImg<unsigned char>& image, LineParams params){
        if (params.is_degeneracy()) {
            image.draw_point(params.x1, params.y1, BLACK);
            return;
        }

        int dx = params.x2 - params.x1;
        int dy = params.y2 - params.y1;

        int sx = sign(dx);
        int sy = sign(dy);
        dx = abs(dx); dy = abs(dy);

        bool flag = dy > dx;
        if (flag)
            swap(dx, dy);

        double f = static_cast<double>(dy) / static_cast<double>(dx) - 0.5;
        int x = params.x1;
        int y = params.y1;

        for (int i = 0; i <= dx; i++) {
            image.draw_point(x, y, BLACK);
            if (f >= 0) {
                if(flag)
                    x += sx;
                else
                    y += sy;
                f -= 1.;
            }
            if (flag)
                y += sy;
            else
                x += sx;
            f += static_cast<double>(dy) / static_cast<double>(dx);
        }
        image.draw_point(params.x2, params.y2, BLACK);
    }
}