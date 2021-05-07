#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "math/hexagonlib.h"
#include "visual/eulertonnetz.h"

class RenderBuffer
{
public:
    RenderBuffer();

    std::unordered_map<int, Hexagon::Hex> TonnetzMap;
    std::vector<Hexagon::Hex> TonnetzGridPositions; // from hexagonlib

    void prepareTonnetzGrid(int minPitch, int maxPitch, int centralMidiPitch);

};

#endif // RENDERBUFFER_H
