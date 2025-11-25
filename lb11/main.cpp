#define cimg_use_png
#define cimg_use_jpeg
#define cimg_display 0

#include "CImg.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>

using namespace cimg_library;

using Image = CImg<unsigned char>;

struct point3d {
    double x;
    double y;
    double z;
};

struct point2d {
    double x;
    double y;
};

using edge_list = std::vector<std::pair<int,int>>;

point2d project_point(const point3d& p,
                      const std::string& projection_mode,
                      double d,
                      int width,
                      int height) {
    double xp = 0.0;
    double yp = 0.0;

    if (projection_mode == "central") {
        xp = d * p.x / p.z;
        yp = d * p.y / p.z;
    } else {
        xp = p.x;
        yp = p.y;
    }

    point2d res;
    res.x = width  / 2.0 + xp;
    res.y = height / 2.0 - yp;
    return res;
}

void draw_wireframe(Image& img,
                    const std::vector<point3d>& verts3d,
                    const edge_list& edges,
                    const std::string& projection_mode,
                    double d,
                    const unsigned char color[3]) {
    const int width  = img.width();
    const int height = img.height();

    std::vector<point2d> verts2d(verts3d.size());
    for (size_t i = 0; i < verts3d.size(); ++i)
        verts2d[i] = project_point(verts3d[i], projection_mode, d, width, height);

    for (const auto& e : edges) {
        int i = e.first;
        int j = e.second;
        if (i < 0 || j < 0 ||
            i >= (int)verts2d.size() || j >= (int)verts2d.size())
            continue;

        img.draw_line(
            (int)std::round(verts2d[i].x),
            (int)std::round(verts2d[i].y),
            (int)std::round(verts2d[j].x),
            (int)std::round(verts2d[j].y),
            color
        );
    }
}

void make_cube(std::vector<point3d>& verts, edge_list& edges) {
    verts.clear();
    edges.clear();

    const double a   = 100.0;
    const double z0  = 600.0; 

    verts.push_back({-a, -a, z0 - a});
    verts.push_back({ a, -a, z0 - a});
    verts.push_back({ a,  a, z0 - a});
    verts.push_back({-a,  a, z0 - a}); 

    verts.push_back({-a, -a, z0 + a});
    verts.push_back({ a, -a, z0 + a}); 
    verts.push_back({ a,  a, z0 + a});
    verts.push_back({-a,  a, z0 + a}); 

    edges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
}

void make_dodecahedron(std::vector<point3d>& verts, edge_list& edges) {
    verts.clear();
    edges.clear();

    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const double a = 1.0;
    const double b = 1.0 / phi;
    const double c = phi;

    for (int sx : {-1, 1})
        for (int sy : {-1, 1})
            for (int sz : {-1, 1})
                verts.push_back({sx * a, sy * a, sz * a});

    for (int sy : {-1, 1})
        for (int sz : {-1, 1})
            verts.push_back({0.0, sy * b, sz * c});

    for (int sx : {-1, 1})
        for (int sy : {-1, 1})
            verts.push_back({sx * b, sy * c, 0.0});

    for (int sx : {-1, 1})
        for (int sz : {-1, 1})
            verts.push_back({sx * c, 0.0, sz * b});

    const double scale    = 80.0;
    const double z_offset = 600.0;

    for (auto& v : verts) {
        v.x *= scale;
        v.y *= scale;
        v.z *= scale;
        v.z += z_offset;
    }

    double edge_len = -1.0;

    for (size_t i = 0; i < verts.size(); ++i) {
        for (size_t j = i + 1; j < verts.size(); ++j) {
            double dx = verts[i].x - verts[j].x;
            double dy = verts[i].y - verts[j].y;
            double dz = verts[i].z - verts[j].z;
            double dist = std::sqrt(dx*dx + dy*dy + dz*dz);

            if (dist > 1e-6)
                if (edge_len < 0.0 || dist < edge_len)
                    edge_len = dist;
        }
    }

    if (edge_len <= 0.0) return;

    const double eps = edge_len * 0.1;

    for (size_t i = 0; i < verts.size(); ++i) {
        for (size_t j = i + 1; j < verts.size(); ++j) {
            double dx = verts[i].x - verts[j].x;
            double dy = verts[i].y - verts[j].y;
            double dz = verts[i].z - verts[j].z;
            double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
            if (std::fabs(dist - edge_len) < eps)
                edges.emplace_back((int)i, (int)j);
        }
    }
}

void print_usage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " <projection_mode> <object_mode> [params]\n\n"
        << "  projection_mode: central | ortho\n"
        << "  object_mode:     cube | dodeca | both\n"
        << "  params (for central): d (distance to projection plane)\n\n"
        << "Examples:\n"
        << "  " << prog << " central both 500\n"
        << "  " << prog << " ortho cube\n"
        << "  " << prog << " central dodeca 400\n";
}

int main(int argc, char** argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    std::string projection_mode = argv[1];
    std::string object_mode     = argv[2];

    if (projection_mode != "central" && projection_mode != "ortho") {
        std::cerr << "Unknown projection_mode: " << projection_mode << "\n";
        print_usage(argv[0]);
        return 1;
    }

    if (object_mode != "cube" &&
        object_mode != "dodeca") {
        std::cerr << "Unknown object_mode: " << object_mode << "\n";
        print_usage(argv[0]);
        return 1;
    }

    double d = 500.0;
    if (projection_mode == "central") {
        if (argc < 4) {
            std::cerr << "central projection requires parameter d\n";
            print_usage(argv[0]);
            return 1;
        }
        d = std::atof(argv[3]);
        if (d <= 0.0) {
            std::cerr << "d must be positive\n";
            return 1;
        }
    }

    const int width  = 800;
    const int height = 600;

    Image img(width, height, 1, 3, 0);

    const unsigned char cube_color[3]    = {255,   0,   0};
    const unsigned char dodeca_color[3]  = {  0, 255,   0};

    if (object_mode == "cube") {
        std::vector<point3d> verts;
        edge_list edges;
        make_cube(verts, edges);
        draw_wireframe(img, verts, edges, projection_mode, d, cube_color);
    }

    if (object_mode == "dodeca") {
        std::vector<point3d> verts;
        edge_list edges;
        make_dodecahedron(verts, edges);
        draw_wireframe(img, verts, edges, projection_mode, d, dodeca_color);
    }

    std::string filename = projection_mode + "_" + object_mode + ".png";
    img.save(filename.c_str());
    std::cout << "Saved: " << filename << "\n";

    return 0;
}
