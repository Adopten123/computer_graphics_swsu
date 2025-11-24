#define cimg_use_png
#define cimg_use_jpeg
#define cimg_display 0

#include "CImg.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>

using namespace cimg_library;

struct point {
    double x, y;
};

using polyline = std::vector<point>;
using Image = CImg<unsigned char>;

void draw_polyline(Image& img, const polyline& poly, const unsigned char color[3], bool closed = false) {
    if (poly.size() < 2) return;
    for (size_t i = 0; i + 1 < poly.size(); ++i)
        img.draw_line((int)std::round(poly[i].x), (int)std::round(poly[i].y), (int)std::round(poly[i+1].x), (int)std::round(poly[i+1].y), color);

    if (closed && poly.size() > 2)
        img.draw_line((int)std::round(poly.back().x), (int)std::round(poly.back().y), (int)std::round(poly[0].x), (int)std::round(poly[0].y), color);
}

void draw_points(Image& img, const polyline& pts, const unsigned char color[3], int radius = 3) {
    for (auto& p : pts)
        img.draw_circle((int)std::round(p.x), (int)std::round(p.y), radius, color);
}

point bezier_quad(const point& P0, const point& P1, const point& P2, double t) {
    double u  = 1.0 - t;
    double u2 = u*u;
    double t2 = t*t;

    point res;
    res.x = u2 * P0.x + 2*t*u * P1.x + t2 * P2.x;
    res.y = u2 * P0.y + 2*t*u * P1.y + t2 * P2.y;
    return res;
}

point bezier_cubic(const point& P0, const point& P1, const point& P2, const point& P3, double t) {
    double u  = 1.0 - t;
    double u2 = u*u;
    double u3 = u2*u;
    double t2 = t*t;
    double t3 = t2*t;

    point res;
    res.x = u3*P0.x + 3*t*u2*P1.x + 3*t2*u*P2.x + t3*P3.x;
    res.y = u3*P0.y + 3*t*u2*P1.y + 3*t2*u*P2.y + t3*P3.y;
    return res;
}

polyline build_quadratic_bezier_curve(const polyline& control, int steps) {
    if (control.size() < 3) return {};

    polyline curve;
    curve.reserve(steps + 1);

    for (int i = 0; i <= steps; ++i)
        curve.push_back(bezier_quad(control[0], control[1], control[2], (double)i / steps));
    return curve;
}

polyline build_cubic_bezier_curve(const polyline& control, int steps) {
    if (control.size() < 4) return {};
    polyline curve;
    curve.reserve(steps + 1);

    for (int i = 0; i <= steps; ++i)
        curve.push_back(bezier_cubic(control[0], control[1], control[2], control[3], (double)i / steps));

    return curve;
}

polyline chaikin_step(const polyline& control) {
    if (control.size() < 2) return control;
    polyline result;
    result.reserve(control.size() * 2);

    for (size_t i = 0; i + 1 < control.size(); ++i) {
        const point& P0 = control[i];
        const point& P1 = control[i+1];

        point Q, R;
        Q.x = 0.75*P0.x + 0.25*P1.x;
        Q.y = 0.75*P0.y + 0.25*P1.y;

        R.x = 0.25*P0.x + 0.75*P1.x;
        R.y = 0.25*P0.y + 0.75*P1.y;

        result.push_back(Q);
        result.push_back(R);
    }

    return result;
}

polyline chaikin_curve(polyline control, int iterations) {
    polyline current = control;
    for (int i = 0; i < iterations; ++i)
        current = chaikin_step(current);
    return current;
}

void print_usage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " <mode> <steps> <x1> <y1> <x2> <y2> ...\n\n"
        << "  mode = 1  - quadratic Bezier (exactly 3 control points)\n"
        << "  mode = 2  - cubic Bezier (exactly 4 control points)\n"
        << "  mode = 3  - Chaikin curve (N >= 2 control points)\n\n"
        << "  steps: for Bezier - number of t-subdivisions; for Chaikin - iterations.\n"
        << "  coordinates: list of pairs x y in pixels.\n\n"
        << "Examples:\n"
        << "  " << prog << " 1 200 100 500 400 100 700 500\n"
        << "  " << prog << " 2 200 100 500 250 100 550 100 700 500\n"
        << "  " << prog << " 3 3 100 500 200 100 400 400 600 100 700 500\n";
}

int main(int argc, char** argv) {
    if (argc < 1 + 2 + 4) { 
        print_usage(argv[0]);
        return 1;
    }

    int mode  = std::atoi(argv[1]);
    int steps = std::atoi(argv[2]);
    if (steps <= 0) {
        std::cerr << "steps must be positive\n";
        return 1;
    }

    int coord_count = argc - 3;
    if (coord_count % 2 != 0) {
        std::cerr << "Number of coordinate arguments must be even (x y x y ...)\n";
        return 1;
    }

    int point_count = coord_count / 2;
    if (point_count < 2) {
        std::cerr << "Need at least 2 points\n";
        return 1;
    }

    if (mode == 1 && point_count != 3) {
        std::cerr << "Mode 1 (quadratic Bezier) requires exactly 3 points\n";
        return 1;
    }
    if (mode == 2 && point_count != 4) {
        std::cerr << "Mode 2 (cubic Bezier) requires exactly 4 points\n";
        return 1;
    }
    if (mode == 3 && point_count < 2) {
        std::cerr << "Mode 3 (Chaikin) requires at least 2 points\n";
        return 1;
    }

    polyline control(point_count);
    int argi = 3;
    for (int i = 0; i < point_count; ++i) {
        control[i].x = std::stod(argv[argi++]);
        control[i].y = std::stod(argv[argi++]);
    }

    int width  = 800;
    int height = 600;
    Image img(width, height, 1, 3, 0);

    const unsigned char control_color[3]   = {255, 0, 0};   
    const unsigned char curve_color[3]     = {0, 255, 0};
    const unsigned char point_color[3]     = {255, 255, 0};

    draw_polyline(img, control, control_color, false);
    draw_points(img, control, point_color, 3);

    if (mode == 1) {
        polyline curve = build_quadratic_bezier_curve(control, steps);
        draw_polyline(img, curve, curve_color, false);
        img.save("bezier_quadratic.png");
        std::cout << "Saved bezier_quadratic.png\n";
    } else if (mode == 2) {
        polyline curve = build_cubic_bezier_curve(control, steps);
        draw_polyline(img, curve, curve_color, false);
        img.save("bezier_cubic.png");
        std::cout << "Saved bezier_cubic.png\n";
    } else if (mode == 3) {
        polyline refined = chaikin_curve(control, steps);
        draw_polyline(img, refined, curve_color, false);
        img.save("chaikin_curve.png");
        std::cout << "Saved chaikin_curve.png\n";
    } else {
        std::cerr << "Unknown mode: " << mode << "\n";
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
