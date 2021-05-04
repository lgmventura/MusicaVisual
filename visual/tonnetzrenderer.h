#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class TonnetzRenderer
{
private:
    int CellDiameter = 10;
//    Orientation Orient = layout_pointy; // from hexagonlib
 //   Point2d Size = Point2d(0,0); // from hexagonlib
//    Layout layout = Layout(layout_pointy, Point(10, 10), Point(0,0)); // from hexagonlib
    std::list<Hexagon::Hex> gridPositions; // from hexagonlib

public:
    TonnetzRenderer(): CellDiameter(10) {};

    enum Shape {Circle, Hexagon};

    void prepareGrid(int maxDist);

    void renderGrid(cv::Mat mat, Shape shape);
    void renderChord(cv::Mat mat, bool *tracks, Shape shape);

    void setCellDiameter(int cellDiam);
    int getCellDiameter() {return this->CellDiameter;};

    void setGridDiameter(int gridDiam, int maxDist);

};


#endif // TONNETZRENDERER_H
