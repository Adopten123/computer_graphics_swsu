#define cimg_use_jpeg
#define cimg_use_png
#define cimg_display 0

#include "CImg.h"
#include <iostream>
#include <cmath>
#include <string>

using namespace cimg_library;
using Image = CImg<unsigned char>;

enum class Interp {
    Nearest,
    Bilinear,
    Bicubic
};

Interp parse_interp(const std::string& s) {
    if (s == "nearest")  return Interp::Nearest;
    if (s == "bilinear") return Interp::Bilinear;
    if (s == "bicubic")  return Interp::Bicubic;
    throw std::runtime_error("Unknown interpolation method: " + s);
}

double get_pixel_clamped(const Image& img, int x, int y, int c) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if (x >= img.width())  x = img.width()  - 1;
    if (y >= img.height()) y = img.height() - 1;
    
    return img(x, y, 0, c);
}

void sample_nearest(const Image& src, double xs, double ys, unsigned char* out) {
    int xi = (int)std::round(xs);
    int yi = (int)std::round(ys);

    for (int c = 0; c < src.spectrum(); ++c)
        out[c] = (unsigned char)get_pixel_clamped(src, xi, yi, c);
}

void sample_bilinear(const Image& src, double xs, double ys, unsigned char* out) {
    int x0 = (int)std::floor(xs);
    int y0 = (int)std::floor(ys);

    double a = xs - x0;
    double b = ys - y0;

    for (int c = 0; c < src.spectrum(); ++c) {
        double f00 = get_pixel_clamped(src, x0,     y0,     c);
        double f10 = get_pixel_clamped(src, x0 + 1, y0,     c);
        double f01 = get_pixel_clamped(src, x0,     y0 + 1, c);
        double f11 = get_pixel_clamped(src, x0 + 1, y0 + 1, c);

        double val =
            (1 - a) * (1 - b) * f00 +
            a       * (1 - b) * f10 +
            (1 - a) * b       * f01 +
            a       * b       * f11;

        if (val < 0) val = 0;
        if (val > 255) val = 255;
        out[c] = (unsigned char)std::round(val);
    }
}

double cubic_weight(double t) {
    t = std::fabs(t);

    const double a = -0.5;
    if (t <= 1.0)
        return (a + 2.0)*t*t*t - (a + 3.0)*t*t + 1.0;
    else if (t < 2.0)
        return a*t*t*t - 5.0*a*t*t + 8.0*a*t - 4.0*a;
    else
        return 0.0;
}

void sample_bicubic(const Image& src, double xs, double ys, unsigned char* out) {
    int x_int = (int)std::floor(xs);
    int y_int = (int)std::floor(ys);

    for (int c = 0; c < src.spectrum(); ++c) {
        double sum = 0.;
        double wsum = 0.;

        for (int m = -1; m <= 2; ++m) {
            double wx = cubic_weight(xs - (x_int + m));
            for (int n = -1; n <= 2; ++n) {
                double wy = cubic_weight(ys - (y_int + n));
                double w = wx * wy;
                double f = get_pixel_clamped(src, x_int + m, y_int + n, c);
                sum  += f * w;
                wsum += w;
            }
        }

        double val = (wsum != 0.0) ? (sum / wsum) : 0.0;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        out[c] = (unsigned char)std::round(val);
    }
}

void sample(const Image& src, double xs, double ys, Interp interp, unsigned char* out) {
    switch (interp) {
        case Interp::Nearest:  sample_nearest(src, xs, ys, out);  break;
        case Interp::Bilinear: sample_bilinear(src, xs, ys, out); break;
        case Interp::Bicubic:  sample_bicubic(src, xs, ys, out);  break;
    }
}

struct Affine {
    double a11, a12;
    double a21, a22;
    double tx, ty;
};

Image warp_affine(const Image& src, int dst_w, int dst_h, const Affine& Ainv, Interp interp) {
    Image dst(dst_w, dst_h, 1, src.spectrum(), 0);

    unsigned char* buf = new unsigned char[src.spectrum()];

    cimg_forXY(dst, x, y) {
        double xs = Ainv.a11 * x + Ainv.a12 * y + Ainv.tx;
        double ys = Ainv.a21 * x + Ainv.a22 * y + Ainv.ty;
        sample(src, xs, ys, interp, buf);
        for (int c = 0; c < src.spectrum(); ++c)
            dst(x, y, 0, c) = buf[c];
    }

    delete[] buf;
    return dst;
}


Image scale_image(const Image& src, double sx, double sy, Interp interp) {
    int dst_w = (int)std::round(src.width()  * sx);
    int dst_h = (int)std::round(src.height() * sy);

    double cx_src = (src.width()  - 1) / 2.0;
    double cy_src = (src.height() - 1) / 2.0;
    double cx_dst = (dst_w        - 1) / 2.0;
    double cy_dst = (dst_h        - 1) / 2.0;

    Affine Ainv;
    Ainv.a11 = 1.0 / sx; Ainv.a12 = 0.0;
    Ainv.a21 = 0.0;      Ainv.a22 = 1.0 / sy;

    Ainv.tx = cx_src - Ainv.a11 * cx_dst - Ainv.a12 * cy_dst;
    Ainv.ty = cy_src - Ainv.a21 * cx_dst - Ainv.a22 * cy_dst;

    return warp_affine(src, dst_w, dst_h, Ainv, interp);
}

Image rotate_image(const Image& src, double angle_deg, Interp interp) {
    double angle_rad = angle_deg * (double)M_PI / 180.0;
    double cosA = std::cos(angle_rad);
    double sinA = std::sin(angle_rad);

    int w = src.width();
    int h = src.height();

    int dst_w = w;
    int dst_h = h;

    double cx = (w - 1) / 2.0;
    double cy = (h - 1) / 2.0;

    Affine Ainv;
    Ainv.a11 =  cosA; Ainv.a12 = sinA;
    Ainv.a21 = -sinA; Ainv.a22 = cosA;

    Ainv.tx = cx - Ainv.a11 * cx - Ainv.a12 * cy;
    Ainv.ty = cy - Ainv.a21 * cx - Ainv.a22 * cy;

    return warp_affine(src, dst_w, dst_h, Ainv, interp);
}

Image shear_x_image(const Image& src, double kx, Interp interp) {
    int w = src.width();
    int h = src.height();

    int dst_w = w;
    int dst_h = h;

    Affine Ainv;
    Ainv.a11 = 1.0;  Ainv.a12 = -kx;
    Ainv.a21 = 0.0;  Ainv.a22 =  1.0;
    Ainv.tx  = 0.0;  Ainv.ty  =  0.0;

    return warp_affine(src, dst_w, dst_h, Ainv, interp);
}

Image shear_y_image(const Image& src, double ky, Interp interp) {
    int w = src.width();
    int h = src.height();

    int dst_w = w;
    int dst_h = h;

    Affine Ainv;
    Ainv.a11 = 1.0;  Ainv.a12 =  0.0;
    Ainv.a21 = -ky;   Ainv.a22 =  1.0;
    Ainv.tx  = 0.0;  Ainv.ty  =  0.0;

    return warp_affine(src, dst_w, dst_h, Ainv, interp);
}

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cerr
            << "Usage:\n"
            << "  " << argv[0] << " input_image output_image transform interpolation [params]\n\n"
            << "  transform:      scale | rotate | shearx | sheary\n"
            << "  interpolation:  nearest | bilinear | bicubic\n\n"
            << "Examples:\n"
            << "  " << argv[0] << " lena.jpg out_scale_bilinear.png scale bilinear 2.0 2.0\n"
            << "  " << argv[0] << " lena.jpg out_rot_bicubic.png   rotate bicubic 45\n"
            << "  " << argv[0] << " lena.jpg out_shear_nearest.png shearx nearest 0.5\n";
        return 1;
    }

    const std::string in_name   = argv[1];
    const std::string out_name  = argv[2];
    const std::string transform = argv[3];
    const std::string interp_s  = argv[4];

    Interp interp = parse_interp(interp_s);
    Image src(in_name.c_str());

    Image dst;

    if (transform == "scale") {
        if (argc < 7) {
            std::cerr << "scale requires sx sy\n";
            return 1;
        }
        double sx = std::stof(argv[5]);
        double sy = std::stof(argv[6]);
        dst = scale_image(src, sx, sy, interp);
    } else if (transform == "rotate") {
        if (argc < 6) {
            std::cerr << "rotate requires angle_deg\n";
            return 1;
        }
        double angle = std::stof(argv[5]);
        dst = rotate_image(src, angle, interp);
    } else if (transform == "shearx") {
        if (argc < 6) {
            std::cerr << "shearx requires kx\n";
            return 1;
        }
        double kx = std::stof(argv[5]);
        dst = shear_x_image(src, kx, interp);
    } else if (transform == "sheary") {
        if (argc < 6) {
            std::cerr << "sheary requires ky\n";
            return 1;
        }
        double ky = std::stof(argv[5]);
        dst = shear_y_image(src, ky, interp);
    } else {
        std::cerr << "Unknown transform: " << transform << "\n";
        return 1;
    }

    dst.save_pnm(out_name.c_str());
    std::cout << "Saved: " << out_name << "\n";

    return 0;
}
