#ifndef HEXAGONLIB_H
#define HEXAGONLIB_H

#include <cmath>
#include <cstdlib>
#include <vector>
//#include <algorithm>
//#include <iterator>
using std::abs;
using std::max;
using std::vector;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Generated code -- CC0 -- No Rights Reserved -- http://www.redblobgames.com/grids/hexagons/
namespace Hexagon {

class Point2d
{
public:
    double x;
    double y;
    //Point2d(double x_, double y_): x(x_), y(y_) {}
    Point2d(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};


class Hex
{
public:
    int q;
    int r;
    int s;
    Hex(int q, int r, int s) {
        this->q = q;
        this->r = r;
        this->s = s;
        if (q + r + s != 0) throw "q + r + s must be 0";
    }
    bool operator< (const Hex& rhs) const
    {
        return (this->q < rhs.q && this->r < rhs.r && this->s < rhs.s);
    }
};


class FractionalHex
{
public:
    double q;
    double r;
    double s;
    FractionalHex(double q, double r, double s) {
        this->q = q;
        this->r = r;
        this->s = s;
        if (round(q + r + s) != 0) throw "q + r + s must be 0";
    }
};


class OffsetCoord
{
public:
    int col;
    int row;
    OffsetCoord(int col_, int row_) {
        this->col = col_;
        this->row = row_;
    }
};


class DoubledCoord
{
public:
    int col;
    int row;
    DoubledCoord(int col_, int row_) {
        this->col = col_;
        this->row = row_;
    }
};


class Orientation
{
public:
    Orientation(double f0_, double f1_, double f2_, double f3_, double b0_, double b1_, double b2_, double b3_, double start_angle_);

    double f0;
    double f1;
    double f2;
    double f3;
    double b0;
    double b1;
    double b2;
    double b3;
    double start_angle;
};


class Layout
{
public:
    Layout();
//    Layout(Orientation orientation_, Point2d size_, Point2d origin_);

    Orientation orientation;
    Point2d size;
    Point2d origin;
};


// Forward declarations


Hex hex_add(Hex a, Hex b);

Hex hex_subtract(Hex a, Hex b);

Hex hex_scale(Hex a, int k);

Hex hex_rotate_left(Hex a);

Hex hex_rotate_right(Hex a);

const vector<Hex> hex_directions = {Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1), Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)};
Hex hex_direction(int direction);

Hex hex_neighbor(Hex hex, int direction);
const vector<Hex> hex_diagonals = {Hex(2, -1, -1), Hex(1, -2, 1), Hex(-1, -1, 2), Hex(-2, 1, 1), Hex(-1, 2, -1), Hex(1, 1, -2)};
Hex hex_diagonal_neighbor(Hex hex, int direction);

int hex_length(Hex hex);

int hex_distance(Hex a, Hex b);

Hex hex_round(FractionalHex h);


FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t);
vector<Hex> hex_linedraw(Hex a, Hex b);



const int EVEN = 1;
const int ODD = -1;
OffsetCoord qoffset_from_cube(int offset, Hex h);


Hex qoffset_to_cube(int offset, OffsetCoord h);


OffsetCoord roffset_from_cube(int offset, Hex h);


Hex roffset_to_cube(int offset, OffsetCoord h);



DoubledCoord qdoubled_from_cube(Hex h);


Hex qdoubled_to_cube(DoubledCoord h);

DoubledCoord rdoubled_from_cube(Hex h);

Hex rdoubled_to_cube(DoubledCoord h);


const Orientation layout_pointy = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation layout_flat = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);
Point2d hex_to_pixel(Layout layout, Hex h);


FractionalHex pixel_to_hex(Layout layout, Point2d p);


Point2d hex_corner_offset(Layout layout, int corner);


vector<Point2d> polygon_corners(Layout layout, Hex h);

} // namespace Hexagon

#endif // HEXAGONLIB_H
