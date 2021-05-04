#ifndef EULERTONNETZ_H
#define EULERTONNETZ_H

#include "math/hexagonlib.h"
#include "musiclib/chords.h"

namespace EulerTonnetz {

Hexagon::Hex getHexagon(Pitch p, bool oneOctave, Pitch central = 60);
std::vector<Hexagon::Hex> getHexagons(Chord c, bool oneOctave, Pitch central = 60);
std::vector<Hexagon::Hex> getHexagonsTracks(Chord c, bool tracks[128], bool oneOctave, Pitch central);
int getMaxDist(int maxPitch, int minPitch, Pitch central);

}

#endif // EULERTONNETZ_H
