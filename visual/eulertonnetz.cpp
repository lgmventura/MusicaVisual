#include "eulertonnetz.h"

EulerTonnetz::EulerTonnetz()
{

}

EulerTonnetz::EulerTonnetz(Pitch centralMidi)
{
    this->CentralMidi = centralMidi;
}

Hex EulerTonnetz::getHexagon(Pitch p, bool oneOctave)
{
    int dist = 0;
    if (oneOctave == true)
    {
        int c1o = this->CentralMidi.getDistanceFromLastC();
        int p1o = p.getDistanceFromLastC();
        dist = p1o - c1o;
    }
    else
    {
        dist = this->CentralMidi.getMidiPitch() - p.getMidiPitch();
    }

    int fifths = dist % 7;
    int majThirds = dist % 4;
    int minThirds = dist % 3;

    int x = +fifths +majThirds;
    int y = -fifths +minThirds;
    int z = -majThirds -minThirds;
    return Hex(x, y, z);
}

std::set<Hex> EulerTonnetz::getHexagons(Chord c, bool oneOctave)
{
    std::set<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        hexagons.insert(this->getHexagon(*it, oneOctave));
    }
}

std::set<Hex> EulerTonnetz::getHexagonsTracks(Chord c, bool *tracks, bool oneOctave)
{
    std::set<Hex> hexagons;
    std::set<Pitch> pitches = c.getPitches();
    for (std::set<Pitch>::iterator it = pitches.begin(); it != pitches.end(); it++)
    {
        Pitch p = (*it);
        if (tracks[p.getMidiTrack()] == true)
        {
            hexagons.insert(this->getHexagon(p, oneOctave));
        }
    }
}

int EulerTonnetz::getMaxDist(int highestPitch, int lowestPitch)
{
    Hex hHex = this->getHexagon(highestPitch, false);
    Hex lHex = this->getHexagon(lowestPitch, false);

    int hcDist = hex_distance(Hex(0,0,0), hHex);
    int lcDist = hex_distance(Hex(0,0,0), lHex);
}
