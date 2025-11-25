#define cimg_display 1
#include "CImg.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace cimg_library;
using namespace std;

const int LEFT   = 1;
const int RIGHT  = 2;
const int BOTTOM = 4;
const int TOP    = 8;

const int IMG_W = 800;
const int IMG_H = 600;

int compute_code(double x, double y,
                double xmin, double ymin,
                double xmax, double ymax)
{
    int code = 0;
    if (x < xmin) code |= LEFT;
    if (x > xmax) code |= RIGHT;
    if (y < ymin) code |= BOTTOM;
    if (y > ymax) code |= TOP;
    return code;
}

bool cohen_sutherland_clip(double &x1, double &y1,
                         double &x2, double &y2,
                         double xmin, double ymin,
                         double xmax, double ymax)
{
    int code1 = compute_code(x1,y1,xmin,ymin,xmax,ymax);
    int code2 = compute_code(x2,y2,xmin,ymin,xmax,ymax);

    while (true)
    {
        if ((code1 | code2) == 0)
            return true;

        if (code1 & code2)
            return false;

        double x, y;
        int outCode = code1 ? code1 : code2;

        if (outCode & TOP) {
            x = x1 + (x2-x1)*(ymax-y1)/(y2-y1);
            y = ymax;
        } else if (outCode & BOTTOM) {
            x = x1 + (x2-x1)*(ymin-y1)/(y2-y1);
            y = ymin;
        } else if (outCode & RIGHT) {
            y = y1 + (y2-y1)*(xmax-x1)/(x2-x1);
            x = xmax;
        } else {
            y = y1 + (y2-y1)*(xmin-x1)/(x2-x1);
            x = xmin;
        }

        if (outCode == code1) {
            x1 = x; y1 = y;
            code1 = compute_code(x1,y1,xmin,ymin,xmax,ymax);
        } else {
            x2 = x; y2 = y;
            code2 = compute_code(x2,y2,xmin,ymin,xmax,ymax);
        }
    }
}

bool midpoint_clip_rec(double x1, double y1,
                     double x2, double y2,
                     double xmin, double ymin,
                     double xmax, double ymax,
                     double eps,
                     double &cx1, double &cy1,
                     double &cx2, double &cy2)
{
    int c1 = compute_code(x1,y1,xmin,ymin,xmax,ymax);
    int c2 = compute_code(x2,y2,xmin,ymin,xmax,ymax);

    if ((c1 | c2) == 0) {
        cx1 = x1; cy1 = y1;
        cx2 = x2; cy2 = y2;
        return true;
    }

    if (c1 & c2) return false;

    if (hypot(x2-x1, y2-y1) < eps) {
        double xm = (x1+x2)/2.0;
        double ym = (y1+y2)/2.0;
        if (compute_code(xm,ym,xmin,ymin,xmax,ymax) == 0) {
            cx1 = cx2 = xm;
            cy1 = cy2 = ym;
            return true;
        }
        return false;
    }

    double xm = (x1+x2)/2.0;
    double ym = (y1+y2)/2.0;

    double ax1,ay1,ax2,ay2;
    double bx1,by1,bx2,by2;

    bool a = midpoint_clip_rec(x1,y1,xm,ym,xmin,ymin,xmax,ymax,
                             eps,ax1,ay1,ax2,ay2);
    bool b = midpoint_clip_rec(xm,ym,x2,y2,xmin,ymin,xmax,ymax,
                             eps,bx1,by1,bx2,by2);

    if (!a && !b) return false;

    if (a && !b) {
        cx1=ax1; cy1=ay1;
        cx2=ax2; cy2=ay2;
        return true;
    }

    if (!a && b) {
        cx1=bx1; cy1=by1;
        cx2=bx2; cy2=by2;
        return true;
    }

    cx1=ax1; cy1=ay1;
    cx2=bx2; cy2=by2;
    return true;
}

bool midpointClip(double &x1, double &y1, double &x2, double &y2,
                  double xmin, double ymin, double xmax, double ymax) {
    double cx1,cy1,cx2,cy2;
    bool ok = midpoint_clip_rec(x1,y1,x2,y2,
                              xmin,ymin,xmax,ymax,
                              0.5, cx1,cy1,cx2,cy2);
    if (ok) {
        x1=cx1; y1=cy1;
        x2=cx2; y2=cy2;
    }
    return ok;
}

struct Segment { double x1,y1,x2,y2; };

int main()
{
    cout << "Input data:\n"
            "xmin ymin xmax ymax n  x1 y1 x2 y2 ... method\n";

    double xmin,ymin,xmax,ymax;
    int n, method;

    cin >> xmin >> ymin >> xmax >> ymax >> n;

    vector<Segment> segs(n);
    for (int i=0;i<n;i++)
        cin >> segs[i].x1 >> segs[i].y1 >> segs[i].x2 >> segs[i].y2;

    cin >> method;

    CImg<unsigned char> img(IMG_W,IMG_H,1,3,255);

    const unsigned char color_window[3]  = {255,0,0};
    const unsigned char color_src[3]     = {150,150,150};
    const unsigned char color_clipped[3] = {0,180,0};

    img.draw_rectangle((int)xmin,(int)ymin,(int)xmax,(int)ymax,
                       color_window,1.0f,~0U);

    for (auto &s: segs)
    {
        img.draw_line((int)s.x1,(int)s.y1,(int)s.x2,(int)s.y2,color_src);

        double x1=s.x1, y1=s.y1, x2=s.x2, y2=s.y2;
        bool vis = (method==1)
                ? cohen_sutherland_clip(x1,y1,x2,y2,xmin,ymin,xmax,ymax)
                : midpointClip(x1,y1,x2,y2,xmin,ymin,xmax,ymax);

        if (vis)
            img.draw_line((int)x1,(int)y1,(int)x2,(int)y2,color_clipped);
    }

    CImgDisplay disp(img, "Processor");
    while (!disp.is_closed()) disp.wait();

    return 0;
}
