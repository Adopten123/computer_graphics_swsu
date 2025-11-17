#define cimg_use_jpeg
#define cimg_use_png
#define cimg_display 0 

#include "CImg.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace cimg_library;
using Image = CImg<unsigned char>;

struct image_data {
    int width, height, spectrum;
};

struct params{
    double R, sigma, T;
};

double compute_luminance(unsigned char r, unsigned char g, unsigned char b) {
    const double LUMA_R_COEF = 0.299;
    const double LUMA_G_COEF = 0.587;
    const double LUMA_B_COEF = 0.114;

    return LUMA_R_COEF  * r + 
           LUMA_G_COEF  * g + 
           LUMA_B_COEF  * b;
}

Image process_low(const Image& image, const image_data& data, const params& param){
    const double cx = (data.width - 1) / 2.;
    const double cy = (data.height - 1) / 2.;
    const double R2 = std::pow(param.R, 2);

    Image blurred = image.get_blur(param.sigma);

    Image lowpass(image, false);
    lowpass = image;

    cimg_forXY(lowpass, x, y) {
        const double dx = x - cx;
        const double dy = y - cy;
        const double dist2 = dx*dx + dy*dy;

        if (dist2 > R2)
            for (int ch = 0; ch < data.spectrum; ++ch) {
                lowpass(x, y, 0, ch) = blurred(x, y, 0, ch);
        }
    }
    return lowpass;
}

Image process_high(const Image& image, const image_data& data, const params& param){
    const int SHARP_KERNEL_SIZE = 3;

    const double SHARP_KERNEL[SHARP_KERNEL_SIZE][SHARP_KERNEL_SIZE] = {
        {  0.0,  -1.0,   0.0 },
        { -1.0,   5.0,  -1.0 },
        {  0.0,  -1.0,   0.0 }
    };

    CImg<double> kernel(SHARP_KERNEL_SIZE, SHARP_KERNEL_SIZE, 1, 1, 0);

    for (int i = 0; i < SHARP_KERNEL_SIZE; ++i)
        for (int j = 0; j < SHARP_KERNEL_SIZE; ++j)
            kernel(j, i) = SHARP_KERNEL[i][j];

    Image sharpened = image.get_convolve(kernel);

    Image highpass(image, false);
    highpass = image;

    cimg_forXY(highpass, x2, y2) {
        unsigned char r = image(x2, y2, 0, 0);
        unsigned char g = image(x2, y2, 0, 1 < data.spectrum ? 1 : 0);
        unsigned char b = image(x2, y2, 0, 2 < data.spectrum ? 2 : 0);
        double Y = compute_luminance(r, g, b);

        if (Y > param.T)
            for (int ch = 0; ch < data.spectrum; ++ch)
                highpass(x2, y2, 0, ch) = sharpened(x2, y2, 0, ch);
    }

    return highpass;
}

int main(int argc, char** argv) {
    if (argc < 7) {
        std::cerr << "Использование:\n"
             << "  " << argv[0]
             << " input_image output_lowpass output_highpass R sigma T\n\n"
             << "где:\n"
             << "  input_image   - входной файл (jpg/png и т.п.)\n"
             << "  output_lowpass  - файл с результатом ФНЧ (гаусс вне круга)\n"
             << "  output_highpass - файл с результатом ФВЧ (резкость по ярким пикселям)\n"
             << "  R       - радиус круга в пикселях\n"
             << "  sigma   - сигма гауссова размытия (например 2.0)\n"
             << "  T       - порог яркости (0..255, например 150)\n"
        << std::endl;
        return 1;
    }

    const char* input_name  = argv[1];
    const char* out_low     = argv[2];
    const char* out_high    = argv[3];
    const params param = {
        .R = std::atof(argv[4]),
        .sigma = std::atof(argv[5]),
        .T = std::atof(argv[6])
    };

    Image image(input_name);
    const image_data data = {image.width(), image.height(), image.spectrum()};
        
    if (data.spectrum < 3) 
        std::cerr << "Ожидается цветное изображение (3 канала)." << std::endl;

    Image lowpass = process_low(image, data, param);
    lowpass.save_pnm_p3(out_low);
        
    Image highpass = process_high(image, data, param);
    highpass.save_pnm_p3(out_high);

    return 0;
}
