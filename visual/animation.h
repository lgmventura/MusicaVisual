#ifndef ANIMATION_H
#define ANIMATION_H

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

//Include project files
#include "layer.h"
#include "videorecorder.h"
#include "musicdata.h"
#include "chords.h"
#include "renderChordStar.h"
#include "tonnetzrenderer.h"

class RenderP
{
public:
    RenderP();
    int vertRange = 40;
    int vertShift = 0;
    bool lines[4] = {1,0,0,0}; // {central line, vlines from time signatures, manual, from metronome track}
    unsigned int beat_measure_manual[2] = {4,2}; // {numerator, denominator} = {x, 2^y}
    bool sep_render[3] = {0,0,0};
    int blur_size[2] = {0,0};
    int blur_size_movnotes[2] = {0,0};
    bool extra_time[2] = {0,1};
    bool hlines = 0;
    int hlines_type = 0;
    int hlines_colour[3] = {50,50,50};
    int hlines_n = 12;
    int hlines_offset = 0;
    int vlines_colour[3] = {50,50,50};
    int vlines_track_n = 0;
    bool half_shift = 0;
    int max_connect_dist = 100;

    cv::LineTypes shapeLineType = cv::LINE_8;
};

class AnimState
{
public:
    int zoom = 1000;
    int xpos = 0;
    int zoom_slider_max = 100;
    int xpos_slider_max = 200;
    int zoom_slider = 1000;
    int xpos_slider = 0;
    double CurrentTime = 0;
    void setXpos(int x) {xpos = x;}
    void setZoom(int z) {zoom = z;}
};

class AnimPainter
{
public:
//    cv::Mat *img;
//    std::vector <cv::Mat> img_buffer_sep_tracks;
//    int zoom;
//    int xpos;
//    int win_width;
//    int win_height;
    TonnetzRenderer Tnr;
    void paintBlocksNoShading( cv::Mat image, MusicData mdt, char* window_name, int startMidiTime, int endMidiTime, int window_width, int window_height);
    void paintBlocks(MusicData mdt, cv::Mat, std::vector <cv::Mat>, int, int, int, int, BlockLayers, RenderP);
    void paintChords(MusicData mdt, cv::Mat, int, int, int, int, ChordLayers chordL, RenderP rProp);
    void paintLayers(MusicData mdt, cv::Mat, std::vector <cv::Mat>, int, int, int, int, std::list<Layer> layers, RenderP);
    void appendFrame(cv::Mat image, VideoRecorder* vRec);
};

#endif // ANIMATION_H
