#ifndef COLOUR_H
#define COLOUR_H

#include <cmath>

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

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
