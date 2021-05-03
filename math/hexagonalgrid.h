#ifndef HEXAGONALGRID_H
#define HEXAGONALGRID_H

#include <cmath>

namespace HexagonalGrid {

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

    bool operator==(const Hexagon &other) const {return Hexagon::Coord == other.Coord;};
    bool operator+(const Hexagon &other);

private:
    // Cube:
    Axial Coord;
};

class Grid // only grid and coordinates
{
public:
    Grid();


private:

};

} // Namespace HexagonalGrid

#endif // HEXAGONALGRID_H
