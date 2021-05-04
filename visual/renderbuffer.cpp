#include "renderbuffer.h"

RenderBuffer::RenderBuffer()
{
    this->prepareTonnetzGrid(10);
}

void RenderBuffer::prepareTonnetzGrid(int radius)
{
    this->TonnetzGridPositions.clear();
    Hexagon::Hex centralHex = Hexagon::Hex(0,0,0);
    this->TonnetzGridPositions = Hexagon::hex_spiral(centralHex, radius);
}
