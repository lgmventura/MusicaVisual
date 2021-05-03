#include "tonnetzrenderer.h"

TonnetzRenderer::TonnetzRenderer()
{

}

void TonnetzRenderer::setCellDiameter(int cellDiam)
{
    this->cellDiameter = cellDiam;
    Point size = Point(cellDiameter, cellDiameter);
    this->layout.size.x = cellDiam;
    this->layout.size.y = cellDiam;
}

void TonnetzRenderer::setGridDiameter(int gridDiam, int maxDist)
{
    this->cellDiameter = gridDiam/maxDist;
    layout.size.x = gridDiam/maxDist;
    layout.size.y = gridDiam/maxDist;
}

void TonnetzRenderer::prepareGrid(int radius)
{
//    gridPositions.clear();
    gridPositions.push_back(Hex(0,0,0));
    Hex cube = Hex(0,0,0);
    for (int d = 1; d < radius; d++)
    {
        for (int j = 0; j < 6; j++)
        {
            for (int k = 0; k < radius; k++)
            {
                gridPositions.push_back(cube);
                Hex cube = hex_neighbor(cube, j);
            }
        }
    }
}

void TonnetzRenderer::renderGrid(cv::Mat mat, Shape shape)
{
    if (shape == Shape::Circle)
    {
        std::list<Hex>::iterator it = this->gridPositions.begin();
        for (int iHex = 0; it != this->gridPositions.end(); it++, iHex++)
        {
            cv::Point cvCentre;
            Point hexCentre = hex_to_pixel(this->layout, (*it));
            cvCentre.x = hexCentre.x;
            cvCentre.y = hexCentre.y;
            cv::circle(mat, cvCentre, this->cellDiameter, cv::Scalar(200,200,200));
        }
    }
}
