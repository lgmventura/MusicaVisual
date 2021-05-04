#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace TonnetzRenderer {

enum Shape {Circle, Hexagon};

void renderGrid(cv::Mat mat, cv::Point centre, std::vector<Hexagon::Hex> GridPositions, int cellDiameter, Hexagon::Layout layout, Shape shape);
void renderChord(cv::Mat mat, cv::Point centre, bool *tracks, Shape shape);

};


#endif // TONNETZRENDERER_H
