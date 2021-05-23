#ifndef COLOUR_H
#define COLOUR_H

#include <cmath>

struct rgb {
    double r;       // percent
    double g;       // percent
    double b;       // percent
    rgb() {}; // empty constructor
    rgb(double r_, double g_, double b_): r(r_), g(g_), b(b_) {}; // constructor
    // defining operations:
    rgb operator+ (const rgb& rhs) const
    {
        return (rgb(this->r + rhs.r, this->g + rhs.g, this->b + rhs.b));
    }
    rgb operator+ (const double& rhs) const
    {
        return (rgb(this->r + rhs, this->g + rhs, this->b + rhs));
    }
    rgb operator* (const double& rhs) const
    {
        return (rgb(this->r * rhs, this->g * rhs, this->b * rhs));
    }
};

typedef rgb rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

//static hsv   rgb2hsv(rgb in);
//static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

#endif // COLOUR_H
