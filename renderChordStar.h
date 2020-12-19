#ifndef RENDERCHORDSTAR_H
#define RENDERCHORDSTAR_H

#include "chords.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>

void renderChordStar(chord chord, chord::circle type, cv::Mat mat, cv::Point centre, int diameter);

void dispChordDisc(chord::circle type, cv::Mat mat, cv::Point centre, int diameter);

#endif // RENDERCHORDSTAR_H
