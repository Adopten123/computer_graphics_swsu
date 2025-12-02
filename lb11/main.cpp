#define cimg_use_png
#define cimg_use_jpeg
#define cimg_display 0

#include "CImg.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace cimg_library;

using Image = CImg<unsigned char>;

struct point3d { double x, y, z; };
struct point2d { double x, y; };

using edge_list = std::vector<std::pair<int,int>>;

point3d rotateX(const point3d& p, double a) {
    double s = std::sin(a), c = std::cos(a);
    return { p.x, c*p.y - s*p.z, s*p.y + c*p.z };
}

point3d rotateY(const point3d& p, double a) {
    double s = std::sin(a), c = std::cos(a);
    return { c*p.x + s*p.z, p.y, -s*p.x + c*p.z };
}

point3d rotateZ(const point3d& p, double a) {
    double s = std::sin(a), c = std::cos(a);
    return { c*p.x - s*p.y, s*p.x + c*p.y, p.z };
}

point2d project_ortho(const point3d& p, int w, int h) {
    return { w/2.0 + p.x, h/2.0 - p.y };
}

void draw_wireframe(Image& img,
                    std::vector<point3d> verts,
                    const edge_list& edges,
                    const unsigned char color[3],
                    double ax, double ay, double az)
{
    int W = img.width();
    int H = img.height();

    for (auto& v : verts) {
        v = rotateX(v, ax);
        v = rotateY(v, ay);
        v = rotateZ(v, az);
    }

    std::vector<point2d> v2(verts.size());
    for (size_t i = 0; i < verts.size(); ++i)
        v2[i] = project_ortho(verts[i], W, H);

    for (auto& e : edges) {
        int i = e.first;
        int j = e.second;
        img.draw_line(
            (int)std::round(v2[i].x),
            (int)std::round(v2[i].y),
            (int)std::round(v2[j].x),
            (int)std::round(v2[j].y),
            color
        );
    }
}

void make_cube(std::vector<point3d>& v, edge_list& e) {
    double a = 120.0;

    v = {
        {-a,-a,-a}, { a,-a,-a}, { a, a,-a}, {-a, a,-a},
        {-a,-a, a}, { a,-a, a}, { a, a, a}, {-a, a, a}
    };

    e = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
}

void make_dodecahedron(std::vector<point3d>& verts, edge_list& edges)
{
    verts.clear();
    edges.clear();

    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    double a = 1.0 / phi;
    double b = phi;

    std::vector<point3d> v = {
        {-1,-1,-1}, {-1,-1, 1}, {-1, 1,-1}, {-1, 1, 1},
        { 1,-1,-1}, { 1,-1, 1}, { 1, 1,-1}, { 1, 1, 1},
        { 0, a, b}, { 0,-a, b}, { 0, a,-b}, { 0,-a,-b},
        { a, b, 0}, {-a, b, 0}, { a,-b, 0}, {-a,-b, 0},
        { b, 0, a}, {-b, 0, a}, { b, 0,-a}, {-b, 0,-a}
    };

    for (auto& p : v) {
        p.x *= 85.0;
        p.y *= 85.0;
        p.z *= 85.0;
    }

    verts = v;

    double minD = 1e9;
    for (int i = 0; i < (int)v.size(); ++i)
        for (int j = i+1; j < (int)v.size(); ++j) {
            double dx = v[i].x - v[j].x;
            double dy = v[i].y - v[j].y;
            double dz = v[i].z - v[j].z;
            double d = std::sqrt(dx*dx+dy*dy+dz*dz);
            if (d > 1e-6 && d < minD) minD = d;
        }

    double eps = minD * 0.15;

    for (int i = 0; i < (int)v.size(); ++i)
        for (int j = i+1; j < (int)v.size(); ++j) {
            double dx = v[i].x - v[j].x;
            double dy = v[i].y - v[j].y;
            double dz = v[i].z - v[j].z;
            double d = std::sqrt(dx*dx+dy*dy+dz*dz);
            if (std::fabs(d - minD) < eps)
                edges.push_back({i, j});
        }
}

int main()
{
    const int W = 900;
    const int H = 700;

    const unsigned char red[3]   = {255, 0, 0};
    const unsigned char green[3] = {0, 180, 0};

    double ax = 0.9, ay = 0.7, az = 0.3;

    {
        Image img(W, H, 1, 3, 255);
        std::vector<point3d> cv;
        edge_list ce;
        make_cube(cv, ce);
        draw_wireframe(img, cv, ce, red, ax, ay, az);
        img.save("cube.png");
    }

    {
        Image img(W, H, 1, 3, 255);
        std::vector<point3d> dv;
        edge_list de;
        make_dodecahedron(dv, de);
        draw_wireframe(img, dv, de, green, ax, ay, az);
        img.save("dodecahedron.png");
    }

    return 0;
}
