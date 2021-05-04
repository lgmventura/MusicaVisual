#include "tonnetzrenderer.h"

using namespace Hexagon;

TonnetzRenderer::TonnetzRenderer()
{
    this->CellDiameter = 10;
    this->Size = Point2d(10,10);
    this->HexLayout = Layout(layout_pointy, Point2d(10, 10), Point2d(0, 0));
    this->prepareGrid(10);
}

void TonnetzRenderer::setCellDiameter(int cellDiam)
{
//    this->cellDiameter = cellDiam;
//    Point size = Point(cellDiameter, cellDiameter);
//    this->layout.size.x = cellDiam;
//    this->layout.size.y = cellDiam;
}

void TonnetzRenderer::setGridDiameter(int gridDiam, int maxDist)
{
//    this->cellDiameter = gridDiam/maxDist;
//    layout.size.x = gridDiam/maxDist;
//    layout.size.y = gridDiam/maxDist;
}

void TonnetzRenderer::prepareGrid(int radius)
{
    this->gridPositions.clear();
    Hex centralHex = Hex(0,0,0);
    this->gridPositions = hex_spiral(centralHex, radius);
}

void TonnetzRenderer::renderGrid(cv::Mat mat, cv::Point centre, Shape shape)
{
    if (shape == Shape::Circle)
    {
        std::vector<Hex>::iterator it = this->gridPositions.begin();
        for (int iHex = 0; it != this->gridPositions.end(); it++, iHex++)
        {
            cv::Point cvCentre;
            Point2d hexCentre = hex_to_pixel(this->HexLayout, (*it));
            cvCentre.x = hexCentre.x;
            cvCentre.y = hexCentre.y;
            cvCentre = cvCentre + centre;
            cv::circle(mat, cvCentre, this->CellDiameter, cv::Scalar(200,200,200));
        }
    }
}
