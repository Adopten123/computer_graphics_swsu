#include "CImg.h"
#include <iostream>
#include <tuple>
#include <string>

using namespace cimg_library;

using Image = CImg<unsigned char>;

std::tuple<Image, Image> make_test_images() {
    Image img1(400, 300, 1, 3);
    Image img2(400, 300, 1, 3);

    cimg_forXY(img1, x, y) {
        img1(x, y, 0) = static_cast<unsigned char>((x * 255) / img1.width());
        img1(x, y, 1) = static_cast<unsigned char>((y * 255) / img1.height());
        img1(x, y, 2) = 128;
    }

    cimg_forXY(img2, x, y) {
        if (((x / 40) + (y / 40)) % 2 == 0) {
            img2(x, y, 0) = 255;
            img2(x, y, 1) = 255;
            img2(x, y, 2) = 255;
        } else {
            img2(x, y, 0) = 0;
            img2(x, y, 1) = 100;
            img2(x, y, 2) = 200;
        }
    }

    try {
        img1.save("input1.bmp");
        img2.save("input2.bmp");
        std::cout << "Test images saved as input1.bmp and input2.bmp" << std::endl;
    } catch (const cimg_library::CImgException& e) {
        std::cerr << "Error saving test images: " << e.what() << std::endl;
    }

    return std::make_tuple(img1, img2);
}

void ProcessImage(const Image& img1, const Image& img2) {
    try {
        Image temp_img1 = img1;
        Image temp_img2 = img2;

        if (temp_img1.width() != temp_img2.width() || temp_img1.height() != temp_img2.height()) {
            std::cout << "Resizing images to match dimensions..." << std::endl;
            temp_img1.resize(temp_img2.width(), temp_img2.height());
        }

        CImg<float> f_img1 = temp_img1.get_normalize(0, 1);
        CImg<float> f_img2 = temp_img2.get_normalize(0, 1);

        CImg<float> hsv = f_img1.RGBtoHSV();

        cimg_forXY(hsv, x, y) {
            float &V = hsv(x, y, 2);
            float &S = hsv(x, y, 1);
            if (V < 0.5f) {
                S *= 1.3f;
                if (S > 1.0f) S = 1.0f;
            }
        }

        CImg<float> processed = hsv.HSVtoRGB();

        float alpha = 0.5f;
        CImg<float> result(processed.width(), processed.height(), 1, 3);
        cimg_forC(result, c) {
            cimg_forXY(result, x, y) {
                result(x, y, c) = processed(x, y, c) * alpha + f_img2(x, y, c) * (1 - alpha);
            }
        }

        result.normalize(0, 255).save_pnm_p3("output.bmp");
        std::cout << "Result saved as output.bmp" << std::endl;
        std::cout << "Processing completed successfully!" << std::endl;

    } catch (const cimg_library::CImgException& e) {
        std::cerr << "Error during image processing: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 3) {
            std::cout << "Loading image 1 from: " << argv[1] << std::endl;
            std::cout << "Loading image 2 from: " << argv[2] << std::endl;

            Image img1(argv[1]);
            Image img2(argv[2]);

            std::cout << "Image 1: " << img1.width() << "x" << img1.height() << std::endl;
            std::cout << "Image 2: " << img2.width() << "x" << img2.height() << std::endl;

            ProcessImage(img1, img2);
        }
        else if (argc == 1) {
            std::cout << "No arguments provided. Creating test images..." << std::endl;

            Image img1, img2;
            std::tie(img1, img2) = make_test_images();
            ProcessImage(img1, img2);
        }
        else {
            std::cerr << "Wrong number of arguments." << std::endl;
            std::cerr << "Usage: " << argv[0] << " [image1 image2]" << std::endl;
            std::cerr << "If no arguments provided, test images will be created." << std::endl;
            return 1;
        }
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 1;
    }

    return 0;
}