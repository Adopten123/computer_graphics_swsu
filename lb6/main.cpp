

#define cimg_display 0

#include "CImg.h"
#include <iostream>
#include <cmath>

using namespace cimg_library;
using Image = CImg<unsigned char>;

struct image_data {
    int width, height, spectrum;
};

struct affine_params {
    double sx, sy, tx, ty;
};

static double read_bilinear(const Image img, double x, double y, double c) {
    if(x < 0. or y < 0. or x > img.width() or y > img.height() - 1.)
        return 255.0;
    return img.linear_atXY(x, y, 0, c);
}

Image process_affine_transformation(Image& src, const image_data data, affine_params params) {
    Image aff_fwd(data.width, data.height, 1, data.spectrum, 255);
    cimg_forXY(aff_fwd, i, j){
        double xs = ((double)i - params.tx) / params.sx;
        double ys = ((double)j - params.ty) / params.sy;
        for(int ch = 0; ch < data.spectrum; ++ch)
            aff_fwd(i,j,0,ch) = (unsigned char)std::round(read_bilinear(src, xs, ys, ch));
    }
    return aff_fwd;
}

Image invert_affine_transformation(Image& src, const image_data data, affine_params params) {
    Image aff_inv(data.width, data.height, 1, data.spectrum, 255);
    cimg_forXY(aff_inv, i, j){
        double xf = params.sx * (double)i + params.tx;
        double yf = params.sy * (double)j + params.ty;
        for(int ch = 0; ch < data.spectrum; ++ch)
            aff_inv(i,j,0,ch) = (unsigned char)std::round(read_bilinear(src, xf, yf, ch));
    }
    return aff_inv;
}

Image process_functional_transformation(Image& src, image_data data, affine_params params) {
    Image func_img(data.width, data.height, 1, data.spectrum, 255);
    cimg_forXY(func_img, i, j){
        double xprime = 0.5 * (double)i;
        double yprime = (double) j;
        for(int ch = 0; ch < data.spectrum; ++ch)
            func_img(i,j,0,ch) = (unsigned char)std::round(read_bilinear(src, xprime, yprime, ch));
    }
    return func_img;
}

int main(int argc, char** argv){
    if (argc !=6) {
        std::cerr << "Usage: " << argv[0] << " input_image sx sy tx ty" << std::endl;
        return 1;
    }

    affine_params params = {
        .sx = std::atof(argv[2]), 
        .sy = std::atof(argv[3]),
        .tx = std::atof(argv[4]),
        .ty = std::atof(argv[5])
    };

    Image image(argv[1]);

    const image_data data = {image.width(), image.height(), image.spectrum()};

    Image aff_fwd = process_affine_transformation(image, data, params);
    aff_fwd.save("aff_fwd.bmp");
    aff_fwd.save_pnm_p3("affine_forward.ppm");

    Image aff_inv = invert_affine_transformation(aff_fwd, data, params);
    aff_inv.save("aff_inv.bmp");
    aff_inv.save_pnm_p3("affine_inverse.ppm");

    Image func_img = process_functional_transformation(image, data, params);
    func_img.save("func_img.bmp");
    func_img.save_pnm_p3("functional_image.ppm");


    return 0;
}