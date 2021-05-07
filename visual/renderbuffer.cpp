#include "renderbuffer.h"

RenderBuffer::RenderBuffer()
{
    this->prepareTonnetzGrid(0,127,60);
}

void RenderBuffer::prepareTonnetzGrid(int minPitch, int maxPitch, int centralPitch)
{
    this->TonnetzMap = EulerTonnetz::createEulerTonnetzMap();
    int maxDist = EulerTonnetz::getMaxDist(minPitch, maxPitch, this->TonnetzMap, centralPitch);
    this->TonnetzGridPositions.clear();
    Hexagon::Hex centralHex = Hexagon::Hex(0,0,0);
    this->TonnetzGridPositions = Hexagon::hex_spiral(centralHex, maxDist);
}
