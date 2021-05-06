#include "eulertonnetz.h"

using namespace Hexagon;

Hex EulerTonnetz::getHexagon(Pitch p, bool oneOctave, Pitch central)
{
    int dist = 0;
    if (oneOctave == true)
    {
        int c1o = central.getDistanceFromLastC();
        int p1o = p.getDistanceFromLastC();
        dist = p1o - c1o;
    }
    else
    {
        dist = central.getMidiPitch() - p.getMidiPitch();
    }
}

std::vector<Hex> EulerTonnetz::getHexagons(Chord c, bool oneOctave, Pitch central)
{
    std::vector<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        hexagons.push_back(EulerTonnetz::getHexagon(*it, oneOctave, central));
    }
    return hexagons;
}

std::vector<Hex> EulerTonnetz::getHexagonsTracks(Chord c, bool *tracks, bool oneOctave, Pitch central)
{
    std::vector<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        Pitch p = (*it);
        if (tracks[p.getMidiTrack()] == true)
        {
            hexagons.push_back(EulerTonnetz::getHexagon(p, oneOctave, central));
        }
    }
    return hexagons;
}

int EulerTonnetz::getMaxDist(int highestPitch, int lowestPitch, Pitch central)
{
    Hex hHex = EulerTonnetz::getHexagon(highestPitch, false, central);
    Hex lHex = EulerTonnetz::getHexagon(lowestPitch, false, central);

    int hcDist = hex_distance(Hex(0,0,0), hHex);
    int lcDist = hex_distance(Hex(0,0,0), lHex);

    return max(hcDist, lcDist);
}

// The directions are used as Hex, but they don't represent the physical positions on the grid
// in this function, but rather the mathematical abstraction of summing a linear combination
// of fifths, major and minor thirds to get any pitch, what the Euler Tonnetz is
std::vector<Hexagon::Hex> getPath(Hexagon::Hex to, Hexagon::Hex from = Hexagon::Hex(0,0,0))
{
    // One direction is +/-7 (fifths), another is +/-4 (maj. thirds) and the other +/-3 (min. thirds)
    // So we end up with 3 directions, 2 ways each, resulting in 6 hex directions:
    vector<Hex> directions = {Hex(3, 0, -3), Hex(7, -7, 0), Hex(0, -4, 4), Hex(-3, 0, 3), Hex(-7, 7, 0), Hex(0, 4, -4)};
    // construct map up to 128+7+4+3, more than this is definitely not needed for 128 possible pitches
    std::unordered_map<Hex, Hex, Hex_hash> hMap = hex_path_map(Hex(0,0,0), 128+7+4+3, directions);
    vector<Hex> path = reconstruct_path(hMap, from, to);
}

// Retrieving the physical positions from the hexagons on the grid given a path
Hexagon::Hex getHexFromPath(std::vector<Hexagon::Hex> path)
{
    Hex hex = Hex(0,0,0);
    vector<Hex>::iterator it = path.begin();
    for (; it != path.end(); it++)
    {
        if ((*it) == Hex(3, 0, -3))
        {
            hex = hex_add(hex, Hex(1, 0, -1));
        }
        else if ((*it) == Hex(-3, 0, 3))
        {
            hex = hex_add(hex, Hex(-1, 0, 1));
        }
        else if ((*it) == Hex(0, 4, -4))
        {
            hex = hex_add(hex, Hex(0, 1, -1));
        }
        else if ((*it) == Hex(0, -4, 4))
        {
            hex = hex_add(hex, Hex(0, -1, 1));
        }
        else if ((*it) == Hex(0, 4, -4))
        {
            hex = hex_add(hex, Hex(0, 1, -1));
        }
        else if ((*it) == Hex(7, -7, 0))
        {
            hex = hex_add(hex, Hex(1, -1, 0));
        }
        else if ((*it) == Hex(-7, 7, 0))
        {
            hex = hex_add(hex, Hex(-1, 1, 0));
        }
    }
}
