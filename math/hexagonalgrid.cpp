#include "hexagonalgrid.h"

Axial HexagonalGrid::cubeToAxial(Cube cube)
{
    Axial axCoord;
    axCoord.q = cube.x;
    axCoord.r = cube.z;
    return axCoord;
}

Cube HexagonalGrid::axialToCube(Axial ax)
{
    Cube cube(0, 0, 0);
    cube.x = ax.q;
    cube.z = ax.r;
    cube.y = -cube.x - cube.z;
    return cube;
}

Cube HexagonalGrid::oddrToCube(Offset oddr)
{
    Cube cube(0, 0, 0);
    cube.x = oddr.col - (oddr.row - (oddr.row & 1)) / 2;
    cube.z = oddr.row;
    cube.y = -cube.x - cube.z;
    return cube;
}

Offset HexagonalGrid::CubeToOddr(Cube cube)
{
    Offset oddr;
    oddr.col = cube.x + (cube.z - (cube.z & 1)) / 2;
    oddr.row = cube.z;
    return oddr;
}

int HexagonalGrid::getIntDistance(Cube hexA, Cube hexB)
{
    int dist = std::max(abs(hexA.x - hexB.x), abs(hexA.y - hexB.y), abs(hexA.z - hexB.z));
    return dist;
}

Cube HexagonalGrid::getCubeDist(Cube hexA, Cube hexB)
{
    Cube dist = Cube(abs(hexA.x - hexB.x), abs(hexA.y - hexB.y), abs(hexA.z - hexB.z));
    return dist;
}


namespace HexagonalGrid {


//
Hexagon::Hexagon()
{
    this->Coord = Axial(0, 0);
}

Hexagon::Hexagon(Axial axCoord)
{
    this->Coord = axCoord;
}

Hexagon::Hexagon(Cube cubeCoord)
{
    this->Coord = HexagonalGrid::cubeToAxial(cubeCoord);
}


Axial Hexagon::getAxialCoord()
{
    return this->Coord;
}

Cube Hexagon::getCubeCoord()
{
    return axialToCube(this->Coord);
}

void Hexagon::setAxialCoord(Axial coord)
{
    this->Coord = coord;
}

void Hexagon::setCubeCoord(Cube coord)
{
    this->Coord = cubeToAxial(coord);
}

//bool operator+(const Hexagon &other)
//{

//    return
//}

}
// Grid:
HexagonalGrid::Grid::Grid()
{

}
