#ifndef HEXAGONALGRID_H
#define HEXAGONALGRID_H

#include <cmath>

namespace HexagonalGrid {

struct Point
{
    double x;
    double y;
    Point(double x_, double y_): x(x_), y(y_) {}
};

struct Axial
{
    int q;
    int r;
    Axial(int q_, int r_): q(q_), r(r_) {}
    Axial() {}
    bool operator==(const Axial &other) const {return (this->q == other.q && this->r == other.r);};
};

struct Cube
{
    int x;
    int y;
    int z;
    Cube(int x_, int y_, int z_): x(x_), y(y_), z(z_) {
        if (x + y + z != 0) throw "q + r + s must be 0";
    }
    bool operator==(const Cube &other) const {return (this->x == other.x && this->y == other.y && this->z == other.z);};
};

struct DoubleCube
{
    double x;
    double y;
    double z;
    DoubleCube(double x_, double y_, double z_): x(x_), y(y_), z(z_) {
        if (x + y + z != 0) throw "q + r + s must be 0";
    }
    bool operator==(const Cube &other) const {return (this->x == other.x && this->y == other.y && this->z == other.z);};
};

struct Offset
{
    int row;
    int col;
    Offset(int row_, int col_): row(row_), col(col_) {}
    Offset() {}
};

Axial cubeToAxial(Cube coord);
Cube axialToCube(Axial coord);
Cube oddrToCube(Offset coord);
Offset CubeToOddr(Cube coord);

int getIntDistance(Cube hexA, Cube hexB);
Cube getCubeDist(Cube hexA, Cube hexB);

class Orientation // only grid and coordinates
{
public:
    Orientation(double f0, double f1, double f2, double f3, double b0, double b1, double b2, double b3, double startAngle);

    double f0;
    double f1;
    double f2;
    double f3;
    double b0;
    double b1;
    double b2;
    double b3;
    double StartAngle;
private:

};

const Orientation layout_pointy = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation layout_flat = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);

class Layout
{
public:
    Layout(Orientation orientation, Point size, Point origin);

    Orientation Orientation = layout_pointy;
    Point Size = Point(1, 1);
    Point Origin = Point(0, 0);
};

Point cubeToPixel(Cube hexagon, Layout layout);
DoubleCube pixelToCube(Point pos, Layout layout);

class Hexagon
{
public:
    Hexagon();
    Hexagon(Axial axCoord);
    Hexagon(Cube cubeCoord);

    // get coordinates:
    Axial getAxialCoord();
    Cube getCubeCoord();
    Offset getOddrCoord();
    // set coordinates
    void setAxialCoord(Axial coord);
    void setCubeCoord(Cube coord);
    Point toPixel();

    bool operator==(const Hexagon &other) const {return Hexagon::Coord == other.Coord;};
    Hexagon operator+(const Hexagon &other);
    Hexagon operator-(const Hexagon &other);

private:
    // Cube:
    Cube Coord = Cube(0, 0, 0);
};


} // Namespace HexagonalGrid

#endif // HEXAGONALGRID_H
