#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace Hexagon;

class TonnetzRenderer
{
private:
    int CellDiameter = 10;
    Point2d Size = Point2d(0,0); // from hexagonlib
    Layout HexLayout = Layout(layout_pointy, Point2d(10, 10), Point2d(0,0)); // from hexagonlib
    std::vector<Hexagon::Hex> gridPositions; // from hexagonlib

public:
    TonnetzRenderer();

    enum Shape {Circle, Hexagon};

    void prepareGrid(int maxDist);

    void renderGrid(cv::Mat mat, cv::Point centre, Shape shape);
    void renderChord(cv::Mat mat, cv::Point centre, bool *tracks, Shape shape);

    void setCellDiameter(int cellDiam);
    int getCellDiameter() {return this->CellDiameter;};

    void setGridDiameter(int gridDiam, int maxDist);

};


#endif // TONNETZRENDERER_H
