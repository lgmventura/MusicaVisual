#include "tonnetzrenderer.h"

TonnetzRenderer::TonnetzRenderer()
{

}

void TonnetzRenderer::prepareGrid(int radius)
{
    gridPositions.clear();
    gridPositions.push_back(Hex(0,0,0));
    Hex cube = Hex(0,0,0);
    for (int d = 1; d < radius; d++)
    {
        for (int j = 0; j < 6; j++)
        {
            for (int k = 0; k < radius; k++)
            {
                gridPositions.push_back(cube);
                cube = hex_neighbor(cube, j);
            }
        }
    }
}
