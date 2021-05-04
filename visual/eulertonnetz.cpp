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

    int fifths = dist % 7;
    int majThirds = dist % 4;
    int minThirds = dist % 3;

    int x = +fifths +majThirds;
    int y = -fifths +minThirds;
    int z = -majThirds -minThirds;
    return Hex(x, y, z);
}

std::set<Hex> EulerTonnetz::getHexagons(Chord c, bool oneOctave, Pitch central)
{
    std::set<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        hexagons.insert(EulerTonnetz::getHexagon(*it, oneOctave, central));
    }
}

std::set<Hex> EulerTonnetz::getHexagonsTracks(Chord c, bool *tracks, bool oneOctave, Pitch central)
{
    std::set<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        Pitch p = (*it);
        if (tracks[p.getMidiTrack()] == true)
        {
            hexagons.insert(EulerTonnetz::getHexagon(p, oneOctave, central));
        }
    }
}

int EulerTonnetz::getMaxDist(int highestPitch, int lowestPitch, Pitch central)
{
    Hex hHex = EulerTonnetz::getHexagon(highestPitch, false, central);
    Hex lHex = EulerTonnetz::getHexagon(lowestPitch, false, central);

    int hcDist = hex_distance(Hex(0,0,0), hHex);
    int lcDist = hex_distance(Hex(0,0,0), lHex);

    return max(hcDist, lcDist);
}
