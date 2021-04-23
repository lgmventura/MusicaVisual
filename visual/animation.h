#ifndef ANIMATION_H
#define ANIMATION_H

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

//Include project files
#include "tracks.h"
#include "videorecorder.h"
#include "musicdata.h"
#include "chords.h"
#include "renderChordStar.h"

class RenderP
{
public:
    RenderP();
    int vertRange = 40;
    int vertShift = 0;
    bool lines[4] = {1,0,0,0}; // {central line, vlines from time signatures, manual, from metronome track}
    unsigned short beat_measure_manual[2] = {4,2}; // {numerator, denominator} = {x, 2^y}
    bool sep_render[3] = {0,0,0};
    short blur_size[2] = {0,0};
    short blur_size_movnotes[2] = {0,0};
    bool extra_time[2] = {0,1};
    bool hlines = 0;
    short hlines_type = 0;
    short hlines_colour[3] = {50,50,50};
    short hlines_n = 12;
    short hlines_offset = 0;
    short vlines_colour[3] = {50,50,50};
    int vlines_track_n = 0;
    bool half_shift = 0;
    bool note_names = 0;
    int note_names_where = 0;
    int turn_chord_circle = 0;
    bool chord_names = 0;
    bool chord_analysis[24] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}; // ToDo: improve this
    bool chord_star = 0;
    bool accidentalSharp = 0; // 0 for flat, 1 for sharp
    chord::circle chord_star_type = chord::circleOfSemitones;
    cv::LineTypes shapeLineType = cv::LINE_8;
};

class AnimState
{
public:
    int zoom;
    int xpos;
    int zoom_slider_max = 100;
    int xpos_slider_max = 200;
    int zoom_slider;
    int xpos_slider;
    double CurrentTime;
    void setXpos(int x) {xpos = x;}
    void setZoom(int z) {zoom = z;}
};

class AnimPainter
{
public:
    cv::Mat *img;
    std::vector <cv::Mat> img_buffer_sep_tracks;
    int zoom;
    int xpos;
    int win_width;
    int win_height;
    void note_blocks_paint( cv::Mat image, MusicData mdt, char* window_name, int startMidiTime, int endMidiTime, int window_width, int window_height);
    void blocks_paint(MusicData mdt, cv::Mat, std::vector <cv::Mat>, int, int, int, int, TracksP, RenderP, VideoRecorder*);
};

#endif // ANIMATION_H