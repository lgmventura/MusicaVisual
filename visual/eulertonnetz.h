#ifndef EULERTONNETZ_H
#define EULERTONNETZ_H

#include "math/hexagonlib.h"
#include "musiclib/chords.h"

class EulerTonnetz
{
public:
    EulerTonnetz();
    EulerTonnetz(Pitch centralMidi);

    Pitch CentralMidi = 60;

    Hexagon::Hex getHexagon(Pitch p, bool oneOctave);
    std::set<Hexagon::Hex> getHexagons(Chord c, bool oneOctave);
    std::set<Hexagon::Hex> getHexagonsTracks(Chord c, bool tracks[128], bool oneOctave);
    int getMaxDist(int maxPitch, int minPitch);
private:
};

#endif // EULERTONNETZ_H
