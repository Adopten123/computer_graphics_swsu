#include <CImg.h>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace cimg_library;
using namespace std;

struct Point {
    float x, y;
};

bool isInside(const Point& a, const Point& b, const Point& p) {
    return (b.x - a.x) * (p.y - a.y) -
           (b.y - a.y) * (p.x - a.x) >= 0;
}

Point intersection(const Point& p1, const Point& p2,
                   const Point& a, const Point& b) {
    float A1 = p2.y - p1.y;
    float B1 = p1.x - p2.x;
    float C1 = A1 * p1.x + B1 * p1.y;

    float A2 = b.y - a.y;
    float B2 = a.x - b.x;
    float C2 = A2 * a.x + B2 * a.y;

    float det = A1 * B2 - A2 * B1;

    return {
        (B2 * C1 - B1 * C2) / det,
        (A1 * C2 - A2 * C1) / det
    };
}

vector<Point> sutherlandHodgman(const vector<Point>& subject,
                                const vector<Point>& clipper) {
    vector<Point> output = subject;

    for (size_t i = 0; i < clipper.size(); i++) {
        Point A = clipper[i];
        Point B = clipper[(i + 1) % clipper.size()];

        vector<Point> input = output;
        output.clear();

        for (size_t j = 0; j < input.size(); j++) {
            Point P = input[j];
            Point Q = input[(j + 1) % input.size()];

            bool Pin = isInside(A, B, P);
            bool Qin = isInside(A, B, Q);

            if (Qin) {
                if (!Pin)
                    output.push_back(intersection(P, Q, A, B));
                output.push_back(Q);
            } else if (Pin) {
                output.push_back(intersection(P, Q, A, B));
            }
        }
    }
    return output;
}

void scanlineFill(CImg<unsigned char>& img,
                  const vector<Point>& poly,
                  const unsigned char color[3]) {
    int h = img.height();

    for (int y = 0; y < h; y++) {
        vector<float> xs;

        for (size_t i = 0; i < poly.size(); i++) {
            Point p1 = poly[i];
            Point p2 = poly[(i + 1) % poly.size()];

            if ((p1.y < y && p2.y >= y) ||
                (p2.y < y && p1.y >= y)) {
                float x = p1.x +
                          (y - p1.y) *
                          (p2.x - p1.x) /
                          (p2.y - p1.y);
                xs.push_back(x);
            }
        }

        sort(xs.begin(), xs.end());

        for (size_t i = 0; i + 1 < xs.size(); i += 2) {
            for (int x = xs[i]; x < xs[i + 1]; x++) {
                img.draw_point(x, y, color);
            }
        }
    }
}

int main() {
    CImg<unsigned char> img(800, 600, 1, 3, 255);

    vector<Point> subjectPoly = {
        {300, 100}, {350, 250}, {500, 250},
        {380, 330}, {450, 480}, {300, 380},
        {150, 480}, {220, 330}, {100, 250},
        {250, 250}
    };

    vector<Point> clipPoly = {
        {200, 150},
        {500, 150},
        {500, 400},
        {200, 400}
    };

    vector<Point> result = sutherlandHodgman(subjectPoly, clipPoly);

    unsigned char fillColor[3] = {255, 0, 0};

    scanlineFill(img, result, fillColor);

    img.display("Polygon Clipping and Filling");

    return 0;
}
