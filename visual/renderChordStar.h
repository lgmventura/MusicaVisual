#ifndef RENDERCHORDSTAR_H
#define RENDERCHORDSTAR_H

#include "chords.h"
#include "utils/colour.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <math.h>

struct ChordStarOptions
{
    Pitch::circle Type;
    int Radius;
    int TurnCircle = 0;
    bool NoteCollapse = false;
    bool NoteFadeOut = false;
};

namespace ChordStar {

void renderChordStar(Chord chord, Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, bool *tracks, int turn = 0);
void renderNote(Pitch note, float noteProgress, rgb colour, cv::Mat mat, cv::Point centre, int circleDiameter, ChordStarOptions opt);

void dispChordDisc(Chord::circle type, cv::Mat mat, cv::Point centre, int diameter, rgb colour, bool dispPitchNames, int turn = 0, bool accidentalSharp = 0);

};

#endif // RENDERCHORDSTAR_H
