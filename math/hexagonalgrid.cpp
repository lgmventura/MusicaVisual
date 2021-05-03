#include "hexagonalgrid.h"

namespace HexagonalGrid {


Axial cubeToAxial(Cube cube)
{
    Axial axCoord;
    axCoord.q = cube.x;
    axCoord.r = cube.z;
    return axCoord;
}

Cube axialToCube(Axial ax)
{
    Cube cube(0, 0, 0);
    cube.x = ax.q;
    cube.z = ax.r;
    cube.y = -cube.x - cube.z;
    return cube;
}

Cube oddrToCube(Offset oddr)
{
    Cube cube(0, 0, 0);
    cube.x = oddr.col - (oddr.row - (oddr.row & 1)) / 2;
    cube.z = oddr.row;
    cube.y = -cube.x - cube.z;
    return cube;
}

Offset CubeToOddr(Cube cube)
{
    Offset oddr;
    oddr.col = cube.x + (cube.z - (cube.z & 1)) / 2;
    oddr.row = cube.z;
    return oddr;
}

int getIntDistance(Cube hexA, Cube hexB)
{
    int dist = std::max(abs(hexA.x - hexB.x), abs(hexA.y - hexB.y), abs(hexA.z - hexB.z));
    return dist;
}

Cube getCubeDist(Cube hexA, Cube hexB)
{
    Cube dist = Cube(abs(hexA.x - hexB.x), abs(hexA.y - hexB.y), abs(hexA.z - hexB.z));
    return dist;
}

Point cubeToPixel(Cube h, Layout layout)
{
    Orientation M = layout.Orientation;
    Point size = layout.Size;
    Point origin = layout.Origin;
    double x = (M.f0 * h.x + M.f1 * h.y) * size.x;
    double y = (M.f2 * h.x + M.f3 * h.y) * size.y;
    return Point(x + origin.x, y + origin.y);
}

DoubleCube pixelToCube(Point p, Layout layout)
{
    Orientation M = layout.Orientation;
    Point size = layout.Size;
    Point origin = layout.Origin;
    Point pt = Point((p.x - origin.x) / size.x, (p.y - origin.y) / size.y);
    double x = M.b0 * pt.x + M.b1 * pt.y;
    double y = M.b2 * pt.x + M.b3 * pt.y;
    return DoubleCube(x, y, -x - y);
}


//
Hexagon::Hexagon()
{
    this->Coord = Cube(0, 0, 0);
}

Hexagon::Hexagon(Cube cubeCoord)
{
    this->Coord = cubeCoord;
}

Hexagon::Hexagon(Axial axCoord)
{
    this->Coord = HexagonalGrid::axialToCube(axCoord);
}


Cube Hexagon::getCubeCoord()
{
    return this->Coord;
}

Axial Hexagon::getAxialCoord()
{
    return cubeToAxial(this->Coord);
}

void Hexagon::setAxialCoord(Axial coord)
{
    this->Coord = axialToCube(coord);
}

void Hexagon::setCubeCoord(Cube coord)
{
    this->Coord = coord;
}

Hexagon Hexagon::operator+(const Hexagon &other)
{
    Hexagon h;
    h.Coord.x = this->Coord.x + other.Coord.x;
    h.Coord.y = this->Coord.y + other.Coord.y;
    h.Coord.z = this->Coord.z + other.Coord.z;
    return h;
}

Hexagon Hexagon::operator-(const Hexagon &other)
{
    Hexagon h;
    h.Coord.x = this->Coord.x - other.Coord.x;
    h.Coord.y = this->Coord.y - other.Coord.y;
    h.Coord.z = this->Coord.z - other.Coord.z;
    return h;
}

//bool operator+(const Hexagon &other)
//{

//    return
//}

}
// Grid:
HexagonalGrid::Orientation::Orientation(double f0, double f1, double f2, double f3, double b0, double b1, double b2, double b3, double startAngle)
{
    this->f0 = f0;
    this->f1 = f1;
    this->f2 = f2;
    this->f3 = f3;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;
    this->b3 = b3;
    this->StartAngle = startAngle;
}

HexagonalGrid::Layout::Layout(class Orientation orientation, Point size, Point origin)
{
    this->Orientation = orientation;
    this->Size = size;
    this->Origin = origin;
}
