#ifndef TONNETZRENDERER_H
#define TONNETZRENDERER_H

#include "eulertonnetz.h"
#include "utils/colour.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

struct TonnetzOptions
{
    enum Shape {Circle, Hexagon};

    TonnetzOptions(Shape shape, bool *tracks, Hexagon::Layout layout);

    Shape Shp;
    Hexagon::Layout Layout = Hexagon::Layout(Hexagon::layout_pointy, Hexagon::Point2d(1,1), Hexagon::Point2d(0,0));
    bool *Tracks;
    Pitch Central = 60;
    int NoteSize = 10;
//    float ChordProgress = 0;
    bool NoteCollapse = false;
    bool NoteFadeOut = false;
};

namespace TonnetzRenderer {

void renderGrid(cv::Mat mat, cv::Point centre, std::vector<Hexagon::Hex> GridPositions, int cellDiameter, Hexagon::Layout layout, TonnetzOptions::Shape shape, rgb colour);
void renderChord(Chord currChord, float chordProgress, cv::Mat mat, cv::Point centre, TonnetzOptions options, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap);
void renderNote(Pitch note, float noteProgress, cv::Mat mat, cv::Point centre, TonnetzOptions options, std::unordered_map<int, Hexagon::Hex> eulerTonnerzMap, rgb colour);

};


#endif // TONNETZRENDERER_H
