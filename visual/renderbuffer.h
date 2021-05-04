#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "math/hexagonlib.h"
#include "visual/eulertonnetz.h"

class RenderBuffer
{
public:
    RenderBuffer();

    std::vector<Hexagon::Hex> TonnetzGridPositions; // from hexagonlib

    void prepareTonnetzGrid(int maxDist);

};

#endif // RENDERBUFFER_H
