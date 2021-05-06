#include "hexagonlib.h"

namespace Hexagon {

Hex hex_add(Hex a, Hex b)
{
    return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
}

Hex hex_subtract(Hex a, Hex b)
{
    return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
}

Hex hex_scale(Hex a, int k)
{
    return Hex(a.q * k, a.r * k, a.s * k);
}

Hex hex_rotate_left(Hex a)
{
    return Hex(-a.s, -a.q, -a.r);
}

Hex hex_rotate_right(Hex a)
{
    return Hex(-a.r, -a.s, -a.q);
}

Hex hex_direction(int direction)
{
    return hex_directions[direction];
}

Hex hex_neighbor(Hex hex, int direction)
{
    return hex_add(hex, hex_direction(direction));
}

Hex hex_diagonal_neighbor(Hex hex, int direction)
{
    return hex_add(hex, hex_diagonals[direction]);
}

int hex_length(Hex hex)
{
    return int((abs(hex.q) + abs(hex.r) + abs(hex.s)) / 2);
}

int hex_distance(Hex a, Hex b)
{
    return hex_length(hex_subtract(a, b));
}


Hex hex_round(FractionalHex h)
{
    int qi = int(round(h.q));
    int ri = int(round(h.r));
    int si = int(round(h.s));
    double q_diff = abs(qi - h.q);
    double r_diff = abs(ri - h.r);
    double s_diff = abs(si - h.s);
    if (q_diff > r_diff && q_diff > s_diff)
    {
        qi = -ri - si;
    }
    else
        if (r_diff > s_diff)
        {
            ri = -qi - si;
        }
        else
        {
            si = -qi - ri;
        }
    return Hex(qi, ri, si);
}

FractionalHex hex_lerp(FractionalHex a, FractionalHex b, double t)
{
    return FractionalHex(a.q * (1.0 - t) + b.q * t, a.r * (1.0 - t) + b.r * t, a.s * (1.0 - t) + b.s * t);
}


vector<Hex> hex_linedraw(Hex a, Hex b)
{
    int N = hex_distance(a, b);
    FractionalHex a_nudge = FractionalHex(a.q + 1e-06, a.r + 1e-06, a.s - 2e-06);
    FractionalHex b_nudge = FractionalHex(b.q + 1e-06, b.r + 1e-06, b.s - 2e-06);
    vector<Hex> results = {};
    double step = 1.0 / max(N, 1);
    for (int i = 0; i <= N; i++)
    {
        results.push_back(hex_round(hex_lerp(a_nudge, b_nudge, step * i)));
    }
    return results;
}

OffsetCoord qoffset_from_cube(int offset, Hex h)
{
    int col = h.q;
    int row = h.r + int((h.q + offset * (h.q & 1)) / 2);
    if (offset != EVEN && offset != ODD)
    {
        throw "offset must be EVEN (+1) or ODD (-1)";
    }
    return OffsetCoord(col, row);
}

Hex qoffset_to_cube(int offset, OffsetCoord h)
{
    int q = h.col;
    int r = h.row - int((h.col + offset * (h.col & 1)) / 2);
    int s = -q - r;
    if (offset != EVEN && offset != ODD)
    {
        throw "offset must be EVEN (+1) or ODD (-1)";
    }
    return Hex(q, r, s);
}

OffsetCoord roffset_from_cube(int offset, Hex h)
{
    int col = h.q + int((h.r + offset * (h.r & 1)) / 2);
    int row = h.r;
    if (offset != EVEN && offset != ODD)
    {
        throw "offset must be EVEN (+1) or ODD (-1)";
    }
    return OffsetCoord(col, row);
}

Hex roffset_to_cube(int offset, OffsetCoord h)
{
    int q = h.col - int((h.row + offset * (h.row & 1)) / 2);
    int r = h.row;
    int s = -q - r;
    if (offset != EVEN && offset != ODD)
    {
        throw "offset must be EVEN (+1) or ODD (-1)";
    }
    return Hex(q, r, s);
}

DoubledCoord qdoubled_from_cube(Hex h)
{
    int col = h.q;
    int row = 2 * h.r + h.q;
    return DoubledCoord(col, row);
}

Hex qdoubled_to_cube(DoubledCoord h)
{
    int q = h.col;
    int r = int((h.row - h.col) / 2);
    int s = -q - r;
    return Hex(q, r, s);
}

DoubledCoord rdoubled_from_cube(Hex h)
{
    int col = 2 * h.q + h.r;
    int row = h.r;
    return DoubledCoord(col, row);
}

Hex rdoubled_to_cube(DoubledCoord h)
{
    int q = int((h.col - h.row) / 2);
    int r = h.row;
    int s = -q - r;
    return Hex(q, r, s);
}

Point2d hex_to_pixel(Layout layout, Hex h)
{
    Orientation M = layout.orientation;
    Point2d size = layout.size;
    Point2d origin = layout.origin;
    double x = (M.f0 * h.q + M.f1 * h.r) * size.x;
    double y = (M.f2 * h.q + M.f3 * h.r) * size.y;
    return Point2d(x + origin.x, y + origin.y);
}

FractionalHex pixel_to_hex(Layout layout, Point2d p)
{
    Orientation M = layout.orientation;
    Point2d size = layout.size;
    Point2d origin = layout.origin;
    Point2d pt = Point2d((p.x - origin.x) / size.x, (p.y - origin.y) / size.y);
    double q = M.b0 * pt.x + M.b1 * pt.y;
    double r = M.b2 * pt.x + M.b3 * pt.y;
    return FractionalHex(q, r, -q - r);
}

Point2d hex_corner_offset(Layout layout, int corner)
{
    Orientation M = layout.orientation;
    Point2d size = layout.size;
    double angle = 2.0 * M_PI * (M.start_angle - corner) / 6.0;
    return Point2d(size.x * cos(angle), size.y * sin(angle));
}

vector<Point2d> polygon_corners(Layout layout, Hex h)
{
    vector<Point2d> corners = {};
    Point2d center = hex_to_pixel(layout, h);
    for (int i = 0; i < 6; i++)
    {
        Point2d offset = hex_corner_offset(layout, i);
        corners.push_back(Point2d(center.x + offset.x, center.y + offset.y));
    }
    return corners;
}

vector<Hex> hex_ring(Hex centre, int radius)
{
    vector<Hex> results;
    Hex cube = hex_add(centre,
                        hex_scale(hex_direction(4), radius));
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < radius; j++)
        {
            results.push_back(cube);
            cube = hex_neighbor(cube, i);
        }
    }
    return results;
}

vector<Hex> hex_spiral(Hex centre, int radius)
{
    vector<Hex> results;
    results.push_back(centre);
        for (int k = 1; k <= radius; k++)
        {
            vector<Hex> ring = hex_ring(centre, k);
            results.insert(results.end(), ring.begin(), ring.end());
        }
        return results;
}

// The function below
// returns a map from hex "pointing" to another hex, which points to another until we reach
// the starting hex "Hex start".
// if no direction is given, then it will use hex_directions, which point to the neighbours,
// looking one by one. If custom directions are given, then, it will only look those directions,
// analogous to a horse in a chess game, which can jump only in the directions
// (3,2), (2,3), (-2,3), (3,-2), (-2,-3), ...
// This only generates a map where there are no obstacles or anything like that. If there are,
// instead of radius, a graph of hexagon neighbours must be given, e.g., another map where each
// hex is associated with its valid directions
std::unordered_map<Hex, Hex, Hex_hash> hex_path_map(Hex start, int radius, vector<Hex> directions)
{
    vector<Hex> frontier;
    frontier.push_back(start);
    std::unordered_map<Hex, Hex, Hex_hash> came_from; // Hex_hash must be defined for unordered_map to work
    came_from[start] = Hex(0, 0, 0);

    while ( ! frontier.empty())
    {
        Hex current = frontier.back();
        frontier.pop_back();
        vector<Hex>::iterator it = directions.begin();
        for (unsigned int iDir = 0; it != directions.end(); it++, iDir++)
        {
            Hex next = hex_add(current, (*it));
            if (next.q > radius || next.r > radius || next.s > radius)
                continue; // skip when greater than radius
            if (came_from.find(next) != came_from.end())
            {
                frontier.push_back(next);
                came_from[next] = current;
            }
        }
    }
    return came_from;
}

std::unordered_map<Hex, int, Hex_hash> hex_path_map(Hex start, std::unordered_map<Hex, vector<int>> hexNeighbours)
{
    vector<Hex> frontier;
    frontier.push_back(start);
    std::unordered_map<Hex, int, Hex_hash> came_from; // Hex_hash must be defined for unordered_map to work
    came_from[start] = -1;

    while ( ! frontier.empty())
    {
        Hex current = frontier.back();
        frontier.pop_back();
        //std::unordered_map<Hex, vector<int>>::iterator it = hexNeighbours.begin();
        vector<int> directionsFromCurrent = hexNeighbours[current];
        vector<int>::iterator it = directionsFromCurrent.begin();
        for (unsigned int iHex = 0; it != directionsFromCurrent.end(); it++, iHex++)
        {
            Hex next = hex_neighbor(current, (*it));
            if (came_from.find(next) != came_from.end())
            {
                frontier.push_back(next);
                came_from[next] = (*it);
            }
        }
    }
    return came_from;
}

vector<Hex> reconstruct_path(std::unordered_map<Hex, Hex, Hex_hash> came_from, Hex start, Hex goal)
{
    Hex current = goal;

    vector<Hex> path;
    while (current != start)
    {
        path.push_back(current);
        current = came_from[current];
    }
    return path;
}

//
//Hex shortest_sum(int destination, vector<Hex> directions)
//{
//    vector<Hex> frontier;
//    frontier.push_back(Hex(0,0,0));
//    std::unordered_map<std::pair<Hex, int>, Hex, Hex_hash> came_from;

//    while ( ! frontier.empty())
//    {
//        Hex current = frontier.back();
//        frontier.pop_back();
//        vector<Hex>::iterator it = directions.begin();
//        for (unsigned int iDir = 0; it != directions.end(); it++, iDir++)
//        {
//            Hex next = hex_add(current, (*it));
//            came_from[next] = current;
//        }
//    }
//}


//// Tests
//#include <iostream>

//void complain(const char* name)
//{
//  std::cout << "FAIL " << name << std::endl;
//}


//void equal_hex(const char* name, Hex a, Hex b)
//{
//    if (!(a.q == b.q && a.s == b.s && a.r == b.r))
//    {
//        complain(name);
//    }
//}


//void equal_offsetcoord(const char* name, OffsetCoord a, OffsetCoord b)
//{
//    if (!(a.col == b.col && a.row == b.row))
//    {
//        complain(name);
//    }
//}


//void equal_doubledcoord(const char* name, DoubledCoord a, DoubledCoord b)
//{
//    if (!(a.col == b.col && a.row == b.row))
//    {
//        complain(name);
//    }
//}


//void equal_int(const char* name, int a, int b)
//{
//    if (!(a == b))
//    {
//        complain(name);
//    }
//}


//void equal_hex_array(const char* name, vector<Hex> a, vector<Hex> b)
//{
//    equal_int(name, a.size(), b.size());
//    for (int i = 0; i < a.size(); i++)
//    {
//        equal_hex(name, a[i], b[i]);
//    }
//}


//void test_hex_arithmetic()
//{
//    equal_hex("hex_add", Hex(4, -10, 6), hex_add(Hex(1, -3, 2), Hex(3, -7, 4)));
//    equal_hex("hex_subtract", Hex(-2, 4, -2), hex_subtract(Hex(1, -3, 2), Hex(3, -7, 4)));
//}


//void test_hex_direction()
//{
//    equal_hex("hex_direction", Hex(0, -1, 1), hex_direction(2));
//}


//void test_hex_neighbor()
//{
//    equal_hex("hex_neighbor", Hex(1, -3, 2), hex_neighbor(Hex(1, -2, 1), 2));
//}


//void test_hex_diagonal()
//{
//    equal_hex("hex_diagonal", Hex(-1, -1, 2), hex_diagonal_neighbor(Hex(1, -2, 1), 3));
//}


//void test_hex_distance()
//{
//    equal_int("hex_distance", 7, hex_distance(Hex(3, -7, 4), Hex(0, 0, 0)));
//}


//void test_hex_rotate_right()
//{
//    equal_hex("hex_rotate_right", hex_rotate_right(Hex(1, -3, 2)), Hex(3, -2, -1));
//}


//void test_hex_rotate_left()
//{
//    equal_hex("hex_rotate_left", hex_rotate_left(Hex(1, -3, 2)), Hex(-2, -1, 3));
//}


//void test_hex_round()
//{
//    FractionalHex a = FractionalHex(0.0, 0.0, 0.0);
//    FractionalHex b = FractionalHex(1.0, -1.0, 0.0);
//    FractionalHex c = FractionalHex(0.0, -1.0, 1.0);
//    equal_hex("hex_round 1", Hex(5, -10, 5), hex_round(hex_lerp(FractionalHex(0.0, 0.0, 0.0), FractionalHex(10.0, -20.0, 10.0), 0.5)));
//    equal_hex("hex_round 2", hex_round(a), hex_round(hex_lerp(a, b, 0.499)));
//    equal_hex("hex_round 3", hex_round(b), hex_round(hex_lerp(a, b, 0.501)));
//    equal_hex("hex_round 4", hex_round(a), hex_round(FractionalHex(a.q * 0.4 + b.q * 0.3 + c.q * 0.3, a.r * 0.4 + b.r * 0.3 + c.r * 0.3, a.s * 0.4 + b.s * 0.3 + c.s * 0.3)));
//    equal_hex("hex_round 5", hex_round(c), hex_round(FractionalHex(a.q * 0.3 + b.q * 0.3 + c.q * 0.4, a.r * 0.3 + b.r * 0.3 + c.r * 0.4, a.s * 0.3 + b.s * 0.3 + c.s * 0.4)));
//}


//void test_hex_linedraw()
//{
//    equal_hex_array("hex_linedraw", {Hex(0, 0, 0), Hex(0, -1, 1), Hex(0, -2, 2), Hex(1, -3, 2), Hex(1, -4, 3), Hex(1, -5, 4)}, hex_linedraw(Hex(0, 0, 0), Hex(1, -5, 4)));
//}


//void test_layout()
//{
//    Hex h = Hex(3, 4, -7);
//    Layout flat = Layout(layout_flat, Point(10.0, 15.0), Point(35.0, 71.0));
//    equal_hex("layout", h, hex_round(pixel_to_hex(flat, hex_to_pixel(flat, h))));
//    Layout pointy = Layout(layout_pointy, Point(10.0, 15.0), Point(35.0, 71.0));
//    equal_hex("layout", h, hex_round(pixel_to_hex(pointy, hex_to_pixel(pointy, h))));
//}


//void test_offset_roundtrip()
//{
//    Hex a = Hex(3, 4, -7);
//    OffsetCoord b = OffsetCoord(1, -3);
//    equal_hex("conversion_roundtrip even-q", a, qoffset_to_cube(EVEN, qoffset_from_cube(EVEN, a)));
//    equal_offsetcoord("conversion_roundtrip even-q", b, qoffset_from_cube(EVEN, qoffset_to_cube(EVEN, b)));
//    equal_hex("conversion_roundtrip odd-q", a, qoffset_to_cube(ODD, qoffset_from_cube(ODD, a)));
//    equal_offsetcoord("conversion_roundtrip odd-q", b, qoffset_from_cube(ODD, qoffset_to_cube(ODD, b)));
//    equal_hex("conversion_roundtrip even-r", a, roffset_to_cube(EVEN, roffset_from_cube(EVEN, a)));
//    equal_offsetcoord("conversion_roundtrip even-r", b, roffset_from_cube(EVEN, roffset_to_cube(EVEN, b)));
//    equal_hex("conversion_roundtrip odd-r", a, roffset_to_cube(ODD, roffset_from_cube(ODD, a)));
//    equal_offsetcoord("conversion_roundtrip odd-r", b, roffset_from_cube(ODD, roffset_to_cube(ODD, b)));
//}


//void test_offset_from_cube()
//{
//    equal_offsetcoord("offset_from_cube even-q", OffsetCoord(1, 3), qoffset_from_cube(EVEN, Hex(1, 2, -3)));
//    equal_offsetcoord("offset_from_cube odd-q", OffsetCoord(1, 2), qoffset_from_cube(ODD, Hex(1, 2, -3)));
//}


//void test_offset_to_cube()
//{
//    equal_hex("offset_to_cube even-", Hex(1, 2, -3), qoffset_to_cube(EVEN, OffsetCoord(1, 3)));
//    equal_hex("offset_to_cube odd-q", Hex(1, 2, -3), qoffset_to_cube(ODD, OffsetCoord(1, 2)));
//}


//void test_doubled_roundtrip()
//{
//    Hex a = Hex(3, 4, -7);
//    DoubledCoord b = DoubledCoord(1, -3);
//    equal_hex("conversion_roundtrip doubled-q", a, qdoubled_to_cube(qdoubled_from_cube(a)));
//    equal_doubledcoord("conversion_roundtrip doubled-q", b, qdoubled_from_cube(qdoubled_to_cube(b)));
//    equal_hex("conversion_roundtrip doubled-r", a, rdoubled_to_cube(rdoubled_from_cube(a)));
//    equal_doubledcoord("conversion_roundtrip doubled-r", b, rdoubled_from_cube(rdoubled_to_cube(b)));
//}


//void test_doubled_from_cube()
//{
//    equal_doubledcoord("doubled_from_cube doubled-q", DoubledCoord(1, 5), qdoubled_from_cube(Hex(1, 2, -3)));
//    equal_doubledcoord("doubled_from_cube doubled-r", DoubledCoord(4, 2), rdoubled_from_cube(Hex(1, 2, -3)));
//}


//void test_doubled_to_cube()
//{
//    equal_hex("doubled_to_cube doubled-q", Hex(1, 2, -3), qdoubled_to_cube(DoubledCoord(1, 5)));
//    equal_hex("doubled_to_cube doubled-r", Hex(1, 2, -3), rdoubled_to_cube(DoubledCoord(4, 2)));
//}


//void test_all()
//{
//    test_hex_arithmetic();
//    test_hex_direction();
//    test_hex_neighbor();
//    test_hex_diagonal();
//    test_hex_distance();
//    test_hex_rotate_right();
//    test_hex_rotate_left();
//    test_hex_round();
//    test_hex_linedraw();
//    test_layout();
//    test_offset_roundtrip();
//    test_offset_from_cube();
//    test_offset_to_cube();
//    test_doubled_roundtrip();
//    test_doubled_from_cube();
//    test_doubled_to_cube();
//}

}
//int main() {
//  test_all();
//}

