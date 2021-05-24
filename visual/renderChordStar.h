#ifndef RENDERCHORDSTAR_H
#define RENDERCHORDSTAR_H

#include "chords.h"
#include "utils/colour.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>

namespace ChordStar {

void renderChordStar(Chord chord, Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, bool *tracks, int turn = 0);
void renderNote(Pitch note, float noteProgress, Pitch::circle type, int radius, rgb colour, cv::Mat mat, cv::Point centre, int circleDiameter, int turn = 0);

void dispChordDisc(Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, rgb colour, bool dispPitchNames, int turn = 0, bool accidentalSharp = 0);

};

#endif // RENDERCHORDSTAR_H
