#include "cda.h"

namespace CDA {
    using namespace cimg_library;

    int compute_l(LineParams params) {
        using namespace std;
        return abs(params.x2 - params.x1) >= abs(params.y2 - params.y1)
                    ? abs(params.x2 - params.x1)
                    : abs(params.y2 - params.y1);
    }

    double compute_delta(int fst, int snd, int L) {
        return static_cast<double>(snd - fst) / L;
    }

    void draw_line(CImg<unsigned char>& image, LineParams params){
        if (params.is_degeneracy()){
            image.draw_point(params.x1, params.y1, BLACK);
            return;
        }

        int L = compute_l(params);
        double dx = compute_delta(params.x1, params.x2, L);
        double dy = compute_delta(params.y1, params.y2, L);

        double x = params.x1 + 0.5 * sign(dx);
        double y = params.y1 + 0.5 * sign(dy);

        for(int i = 1; i < L + 1; i++){
            x+=dx; y+=dy;
            image.draw_point(std::floor(x), std::floor(y), BLACK);
        }
    }

}