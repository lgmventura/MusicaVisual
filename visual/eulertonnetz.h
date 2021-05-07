#ifndef EULERTONNETZ_H
#define EULERTONNETZ_H

#include "math/hexagonlib.h"
#include "musiclib/chords.h"
#include <unordered_map>
#include <numeric> // alternatively, define a differetiate (adjascent difference)

namespace EulerTonnetz {

Hexagon::Hex getHexagon(Pitch p, bool oneOctave, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, Pitch central = 60);
std::vector<Hexagon::Hex> getHexagons(Chord c, bool oneOctave, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, Pitch central = 60);
std::vector<Hexagon::Hex> getHexagonsTracks(Chord c, bool tracks[128], bool oneOctave, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, Pitch central);
int getMaxDist(int maxPitch, int minPitch, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, Pitch central);
//std::vector<Hexagon::Hex> getPath(Hexagon::Hex to, Hexagon::Hex from = Hexagon::Hex(0,0,0));
//Hexagon::Hex getHexFromPath(std::vector<Hexagon::Hex> path);
std::unordered_map<int, int> createMap(int radius, std::vector<int> directions);
std::vector<int> getPath(std::unordered_map<int, int> map, int goal);
// the following function just bakes the process to allow for faster render:
std::unordered_map<int, Hexagon::Hex> createEulerTonnetzMap();

}

#endif // EULERTONNETZ_H
