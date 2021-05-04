#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace TonnetzRenderer {

enum Shape {Circle, Hexagon};

void renderGrid(cv::Mat mat, cv::Point centre, std::vector<Hexagon::Hex> GridPositions, int cellDiameter, Hexagon::Layout layout, Shape shape);
void renderChord(Chord currChord, cv::Mat mat, cv::Point centre, Hexagon::Layout layout, bool *tracks, Shape shape, int size, Pitch central = 60);

};


#endif // TONNETZRENDERER_H
