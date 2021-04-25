#ifndef RENDERCHORDSTAR_H
#define RENDERCHORDSTAR_H

#include "chords.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>

void renderChordStar(chord chord, chord::circle type, cv::Mat mat, cv::Point centre, int diameter, bool *tracks, int turn = 0);

void dispChordDisc(chord::circle type, cv::Mat mat, cv::Point centre, int diameter, bool dispPitchNames, int turn = 0, bool accidentalSharp = 0);

#endif // RENDERCHORDSTAR_H
