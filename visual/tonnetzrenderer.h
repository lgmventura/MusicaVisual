#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class TonnetzRenderer
{
public:
    TonnetzRenderer();

    enum Shape {Circle, Hexagon};

    void prepareGrid(int maxDist);

    void renderGrid(cv::Mat mat, Shape shape);
    void renderChord(cv::Mat mat, bool *tracks, Shape shape);

    void setCellDiameter(int cellDiam);
    int getCellDiameter() {return this->cellDiameter;};

    void setGridDiameter(int gridDiam, int maxDist);
private:
    int cellDiameter = 10;
    Orientation orientation = layout_pointy; // from hexagonlib
    Point size = Point(cellDiameter, cellDiameter); // from hexagonlib
    Layout layout = Layout(orientation, size, Point(0,0)); // from hexagonlib
    std::list<Hex> gridPositions = {}; // from hexagonlib
};

#endif // TONNETZRENDERER_H
