#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class TonnetzRenderer
{
public:
    TonnetzRenderer();

    enum Shape {Circle, Hexagon};

    void prepareGrid(int maxDist);

    void renderGrid(cv::Mat mat, Shape shape);
    void renderChord(cv::Mat mat, bool *tracks, Shape shape);

    void setCellDiameter(int cellDiam) {this->cellDiameter = cellDiam;};
    int getCellDiameter() {return this->cellDiameter;};

    void setGridDiameter(int gridDiam, int maxDist) {this->cellDiameter = gridDiam/maxDist;};
private:
    int cellDiameter = 10;
    std::list<Hex> gridPositions;
};

#endif // TONNETZRENDERER_H
