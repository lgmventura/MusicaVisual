#ifndef ANIMATION_H
#define ANIMATION_H

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

//Include project files
#include "layercontainer.h"
#include "videorecorder.h"
#include "visual/renderbuffer.h"
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

struct ShapePoints
{
    cv::Point pt1, pt2, pt3, pt4;//, pt5;// , pt6;
    cv::Point pt5[128]; // for the interconnected lines. It has to be a vector of tracks_count length to make lines be connected only with notes within the same track.
    cv::Point pt1_prev[128], pt2_prev[128], pt3_prev[128]; // these points are used for storing the previous respective points to make "moving notes" possible averaging current position/size with last position/size. There must be one for each track so that they are kept independent, i.e., moving notes in one track don't influence mov. notes in another (same idea as for interconn. lines).
    cv::Point PtosPrev[128][4];
    int RadiusPrev[128];// = 0;
    float x1, x2, y1, y2, x3, y3;
    int x1_mov, x2_mov, x3_mov, y1_mov, y2_mov, y3_mov;

    ShapePoints() {
        for (int i = 0; i <128; ++i) {this->pt5[i].x = 0; this->pt5[i].y = 0;}
    }
};

struct AnimWindow
{
    int StartMidiTime; // midi time in ticks for which block notes start to be visible at the right side
    int EndMidiTime; // midi time in ticks for which block notes fade out at the left side
    int Width;
    int Height;
    int CurrPosMiddle;
    float VZoom;
};

class AnimPainter
{
public:
    AnimPainter(RenderBuffer *rBuffer);
//    cv::Mat *img;
//    std::vector <cv::Mat> img_buffer_sep_tracks;
//    int zoom;
//    int xpos;
//    int win_width;
//    int win_height;
    RenderBuffer *RBuffer;

    void paintBlocksNoShading( cv::Mat image, MusicData mdt, ShapePoints spts, char* window_name, AnimWindow aw);
    void paintNotes(MusicData mdt, cv::Mat, std::vector <cv::Mat>, cv::Mat, cv::Mat, AnimWindow aw, BlockLayers, ChordLayers, RenderP, LayerContainer::LayerType ltype);
    void paintChords(Chord chord, float chordProgress, cv::Mat, AnimWindow aw, ChordLayers chordL, RenderP rProp);
    void paintLayers(MusicData mdt, cv::Mat, std::vector <cv::Mat>, cv::Mat, cv::Mat, AnimWindow aw, std::list<LayerContainer> layers, RenderP);
    void appendFrame(cv::Mat image, VideoRecorder* vRec);
};

#endif // ANIMATION_H
