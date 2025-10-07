#include "svgprocessor_utils.h"

std::vector<std::vector<double>> compute_coords(int width, int height) {
    return {
        {width * 0.25, height * 0.25},
        {width * 0.75, height * 0.25},
        {width * 0.25, height * 0.75},
        {width * 0.75, height * 0.75}
    };
}
