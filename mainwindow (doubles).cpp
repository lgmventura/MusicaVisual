/*
 *  This software takes a midi file and creates animations from its notes.
    Copyright (C) 2016  Luiz Guilherme de Medeiros Ventura, Belo Horizonte, Brazil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Very, very, very helpful on working with midi messages: http://midifile.sapp.org/class/MidiMessage/
 * Thanks to Craiggsappmidi (sapp.org) for the library used here.
 * Thanks to OpenCV library used as well to work with images and videos.
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <array>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

//Include library for midi files
#include "MidiFile.h"
#include "Options.h"

#include "dockwidgettracks.h"
#include "midimessages.h"
#include "animationbar.h"

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

// Iostream (just to help on debugging)
#include <iostream>

using namespace std;

TracksP::TracksP()
{

}
RenderP::RenderP()
{

}

TracksP *tracksproperties = new TracksP(); // Global variable with tracks parameters for exhibition.
int n_events; // Global var to store number of events
std::list <MidiNote> notes; // List of processed notes.
std::list <TempoChange> *tempos = new std::list <TempoChange>;
unsigned long total_time = 0; // Global variable to store the total time of the current processed midi file.
unsigned int pitch_max = 1, pitch_min = 100;
unsigned int tracks_count = 1;
unsigned int tpq; // Ticks per Quarter-Note

AnimationBar *animbar;
AnimwinP *animwinP;
AnimPainter *animPt;

RenderP *renderproperties = new RenderP();

//std::string videoFileName;
bool *videoRecord = new bool(false);
cv::VideoWriter *video;
std::string *codec_fourcc = new std::string("X264");

//cv::Point pt3,pt4; // Points for the central line
typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

rgb getColorTrackP(int track, int pitch)
{
    if (tracksproperties->colorScheme[track] == 0) // Just one color for the track
    {
        rgb rgbcolor;
        rgbcolor.r = tracksproperties->getCv(track,0);
        rgbcolor.g = tracksproperties->getCv(track,1);
        rgbcolor.b = tracksproperties->getCv(track,2);
        return (rgbcolor);
    }
    if (tracksproperties->colorScheme[track] == 1) // Base color for the track and a certain pitch and, from them, increase 1/12 hue for each half note
    {
        rgb rgbcolor;
        rgbcolor.b = (double)tracksproperties->getCv(track,2)/255;
        rgbcolor.g = (double)tracksproperties->getCv(track,1)/255;
        rgbcolor.r = (double)tracksproperties->getCv(track,0)/255;
        hsv hsvcolor = rgb2hsv(rgbcolor);
        hsvcolor.h = (double)((int)(hsvcolor.h + 180*pitch/12)%180);// - 180*(int)hsvcolor.h/180;
        rgbcolor = hsv2rgb(hsvcolor);
        rgbcolor.r = rgbcolor.r*255;
        rgbcolor.g = rgbcolor.g*255;
        rgbcolor.b = rgbcolor.b*255;
        return (rgbcolor);
    }
    if (tracksproperties->colorScheme[track] == 2) // Base color for the track and a certain pitch and, from them, increase 5/12 hue for each half note
    {
        rgb rgbcolor;
        rgbcolor.b = (double)tracksproperties->getCv(track,2)/255;
        rgbcolor.g = (double)tracksproperties->getCv(track,1)/255;
        rgbcolor.r = (double)tracksproperties->getCv(track,0)/255;
        hsv hsvcolor = rgb2hsv(rgbcolor);
        hsvcolor.h = (double)((int)(hsvcolor.h + 900*pitch/12)%180); // 180*5*pitch/12
        rgbcolor = hsv2rgb(hsvcolor);
        rgbcolor.r = rgbcolor.r*255;
        rgbcolor.g = rgbcolor.g*255;
        rgbcolor.b = rgbcolor.b*255;
        return (rgbcolor);
    }

}

void AnimPainter::blocks_paint(cv::Mat image, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2, pt3, pt4;
    pt4.x = 0;
    pt4.y = 0;
    double x1, x2, y1, y2, x3, y3;
    int x_max = window_width + 5000, y_max = window_height + 50;
    //std::cout << "Paint blocks! " << pitch_min << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    cv::Mat img_buffer_sep_tracks[tracks_count] = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        x1 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_on)/((double)endMidiTime - (double)startMidiTime));
        x2 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_off)/((double)endMidiTime - (double)startMidiTime));
        y1 = (double)window_height - (double)window_height*((double)(*it).pitch/((double)pitch_max - (double)pitch_min + 20.0));
        y2 = (double)window_height - (double)window_height*(((double)(*it).pitch + 1.0)/((double)pitch_max - (double)pitch_min + 20.0));
        pt1.x = (int)(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
        pt2.x = (int)(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
        pt1.y = (int)(y1); //window_height*((*it).pitch/(50));
        pt2.y = (int)(y2);//window_height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
        if (std::abs(pt1.x) < x_max && std::abs(pt1.y) < y_max && std::abs(pt2.x) < x_max && std::abs(pt2.y) < y_max) // This "if" is just to bypass an issue (certainly about big doubles being coverted to int), what causes random boxes appearing on the screen when one zooms in very close.
        {
            for (short tnum = 0; tnum < (tracks_count); tnum++)
            {
                // -------------------------- Draw Interconnected Lines -------------------------
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true && tracksproperties->interconnect[tnum] == 1) // All tracks - rectangle
                {
                    x3 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_middle)/((double)endMidiTime - (double)startMidiTime));
                    y3 = (double)window_height - (double)window_height*(((double)(*it).pitch + 0.5)/((double)pitch_max - (double)pitch_min + 20.0));
                    pt3.x = (int)x3; // Center x
                    pt3.y = (int)y3; // Center y
                    if (abs(pt3.x - pt4.x) < tracksproperties->max_connect_dist)
                        if ( ! renderproperties->sep_render[1])
                            cv::line( image, pt3, pt4, {tracksproperties->getCv(tnum,2)*(*it).vel/512,  tracksproperties->getCv(tnum,1)*(*it).vel/512,  tracksproperties->getCv(tnum,0)*(*it).vel/512}, 1, 8 );
                        else
                            cv::line( img_buffer_sep_tracks[tnum], pt3, pt4, {tracksproperties->getCv(tnum,2)*(*it).vel/512,  tracksproperties->getCv(tnum,1)*(*it).vel/512,  tracksproperties->getCv(tnum,0)*(*it).vel*512}, 1, 8 );
                    pt4.x = (int)x3; // Center x of the previous note
                    pt4.y = (int)y3; // Center y of the previous note
                }



                // ------------------------- Draw Rectangles -------------------------
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 0) // All tracks - rectangle
                {
                    if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                        if ( ! renderproperties->sep_render[1])
                            cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else
                            cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                    else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                    {
                        if ( ! renderproperties->sep_render[0])
                            cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                        else
                            cv::rectangle( img_playing_notes, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                    }
                    else // The note block is after (to the left of) the center line
                        if ( ! renderproperties->sep_render[1])
                            cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                        else
                            cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                }


                // -------------------------- Draw Rhombus -------------------------
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 1) // All tracks - rectangle
                {
                    x3 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_middle)/((double)endMidiTime - (double)startMidiTime));
                    y3 = (double)window_height - (double)window_height*(((double)(*it).pitch + 0.5)/((double)pitch_max - (double)pitch_min + 20.0));
                    pt1.y = (int)y3;
                    pt2.y = (int)y3;
                    pt3.x = (int)x3;
                    pt3.y = (int)y1;
                    pt4.x = (int)x3;
                    pt4.y = (int)y2;
                    cv::Point ptos[4];
                    ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                    if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                        if ( ! renderproperties->sep_render[1])
                            //cv::fillPoly( image, pts, );
                            cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else
                            cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                    else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                    {
                        if ( ! renderproperties->sep_render[0])
                            cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                        else
                            cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                    }
                    else // The note block is after (to the left of) the center line
                        if ( ! renderproperties->sep_render[1])
                            cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                        else
                            cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                }


                // -------------------------- Draw Ellipses -------------------------
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 2) // All tracks - rectangle
                {
                    x3 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_middle)/((double)endMidiTime - (double)startMidiTime));
                    y3 = (double)window_height - (double)window_height*(((double)(*it).pitch + 0.5)/((double)pitch_max - (double)pitch_min + 20.0));
                    pt3.x = (int)x3; // Center x
                    pt3.y = (int)y3; // Center y
                    if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                        if ( ! renderproperties->sep_render[1])
                            //cv::fillPoly( image, pts, );
                            cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else
                            cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                    else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                    {
                        if ( ! renderproperties->sep_render[0])
                            cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                        else
                            cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                    }
                    else // The note block is after (to the left of) the center line
                        if ( ! renderproperties->sep_render[1])
                            cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                        else
                            cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                }


                // -------------------------- Draw Circles -------------------------
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 3) // All tracks - rectangle
                {
                    x3 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_middle)/((double)endMidiTime - (double)startMidiTime));
                    y3 = (double)window_height - (double)window_height*(((double)(*it).pitch + 0.5)/((double)pitch_max - (double)pitch_min + 20.0));
                    pt3.x = (int)x3; // Center x
                    pt3.y = (int)y3; // Center y
                    int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                    if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                        if ( ! renderproperties->sep_render[1])
                            //cv::fillPoly( image, pts, );
                            cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else
                            cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                    else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                    {
                        if ( ! renderproperties->sep_render[0])
                            cv::circle( image, pt3, (x2-window_width/2+3), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                        else
                            cv::circle( img_playing_notes, pt3, (x2-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                    }
                    else // The note block is after (to the left of) the center line
                        if ( ! renderproperties->sep_render[1])
                            cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                        else
                            cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                }
            }
        }
    }
//    pt3.x = window_width/2;
//    pt4.x = window_width/2;
//    pt3.y = window_height;
//    pt4.y = 0;
    if (renderproperties->lines[0])
        cv::line(image, cv::Point(window_width/2, window_height), cv::Point(window_width/2, 0), {100,100,100});
    if (renderproperties->lines[1])
    {
        for (unsigned int i = 0; i < endMidiTime; i = i + renderproperties->beat_measure_manual[0]*tpq/renderproperties->beat_measure_manual[1])
        {
            cv::line(image, cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {60,60,60});
        }
    }
    if (renderproperties->sep_render[0])
    {
        if (renderproperties->blur_size > 0)
            cv::boxFilter(img_playing_notes, img_playing_notes, -1, cv::Size(renderproperties->blur_size, renderproperties->blur_size));
        image = image + img_playing_notes;
    }
    if (renderproperties->sep_render[1])
    {
        for (short j = 0; j < (tracks_count); j++)
        {
            if (tracksproperties->track_blur[j] > 0)
                cv::boxFilter(img_buffer_sep_tracks[j], img_buffer_sep_tracks[j], -1, cv::Size(tracksproperties->track_blur[j], tracksproperties->track_blur[j]));
            image = image + img_buffer_sep_tracks[j];
        }
    }
    if (*videoRecord == 1 && video != nullptr)
    {
        video->write(image);
    }
}

//================================================================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //TracksP tracksproperties;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImport_MIDI_File_triggered()
{
    QString QmidiFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open MIDI File"),
                "//home//",
                "Midi Files (*.mid *.midi);;All Files (*.*)");

    QByteArray ba = QmidiFileName.toUtf8();
    const char *midiFileName = ba.data();

//    Options options;
//       options.process(midiFileName);//(argc, argv);
//       MidiFile midifile;
//       if (options.getArgCount() > 0) {
//          midifile.read(options.getArg(1));
//       } else {
//          midifile.read(cin);
//       }
    MidiFile midifile;
    midifile.read(midiFileName);

    ui->plainTextEdit->clear(); // unless you know the editor is empty
    //ui->plainTextEdit->appendPlainText(midiFileName);


//        ui->plainTextEdit->appendPlainText("TPQ: ");
//        ui->plainTextEdit->appendPlainText(QString::number(midifile.getTicksPerQuarterNote()));
//        ui->plainTextEdit->appendPlainText("TRACKS: ");
//        ui->plainTextEdit->appendPlainText(QString::number(midifile.getTrackCount()));
       std::stringstream buffer;
       buffer << "TPQ: " << midifile.getTicksPerQuarterNote() << endl; // Get the Ticks Per QarterNote (TPQ), necessary for setting the tempo
       buffer << "TRACKS: " << midifile.getTrackCount() << endl;

       midifile.joinTracks();
       // midifile.getTrackCount() will now return "1", but original
       // track assignments can be seen in .track field of MidiEvent.

//       ui->plainTextEdit->appendPlainText("TICK    DELTA   TRACK   MIDI MESSAGE\n");
//       ui->plainTextEdit->appendPlainText("____________________________________\n");
       buffer << "TICK    DELTA   TRACK   MIDI MESSAGE\n";
       buffer << "____________________________________\n";
       ui->plainTextEdit->appendPlainText(QString::fromStdString(buffer.str()));

       MidiEvent* mev;
       int deltatick;
       for (int event=0; event < midifile[0].size(); event++) {
          mev = &midifile[0][event];
          if (event == 0) {
             deltatick = mev->tick;
          } else {
             deltatick = mev->tick - midifile[0][event-1].tick;
          }
//          ui->plainTextEdit->appendPlainText(QString::number(qint64(dec)));
//          ui->plainTextEdit->appendPlainText(QString::number(mev->tick));
//          ui->plainTextEdit->appendPlainText("\t");
//          ui->plainTextEdit->appendPlainText(QString::number(deltatick));
//          ui->plainTextEdit->appendPlainText("\t");
//          ui->plainTextEdit->appendPlainText(QString::number(mev->track));
//          ui->plainTextEdit->appendPlainText("\t");
//          //string a = << hex;
//          ui->plainTextEdit->appendPlainText((hex));
          std::stringstream buffer;
          buffer << dec << mev->tick;
          buffer << '\t' << deltatick;
          buffer << '\t' << mev->track;
          buffer << '\t' << hex;
          for (int i=0; i < mev->size(); i++) {
 //             ui->plainTextEdit->appendPlainText(QString::number((*mev)[i]));
             buffer << (int)(*mev)[i] << ' ';
             if ((int)(*mev)[i] < 16) buffer << ' '; // this line adds a new space in case of (int) < 16 (or (hex) < 10) to ensure equal spacing between elements of messages.
          }
          buffer << '\t'; // Places a '\t' after each line.
          ui->plainTextEdit->appendPlainText(QString::fromStdString(buffer.str()));
//          cout << endl;
          n_events = event; // number of events in file opened. Calculated after file importing.
       }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString QmvfFileName = QFileDialog::getSaveFileName(
                this,
                tr("Save MusicaVisual File"),
                "//home//",
                "Musica Visual (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");

    QByteArray ba = QmvfFileName.toLatin1();
    const char *mvfFileName = ba.data();

    if( QmvfFileName.isEmpty()==false )
    {
      QFile scriptFile(QmvfFileName);
      if( !scriptFile.open(QIODevice::WriteOnly) )
      {
        QString msg = QString( tr("Unable to save current script to file: %1") ).arg(QmvfFileName);
        //Logger::GetLogger()->WriteToLog( msg );
        QMessageBox::critical(this, tr("Saving path is blank..."), msg, QMessageBox::Ok );
        return;
      }

      // Save the script
      QTextStream stream(&scriptFile);//   <-----
      stream << ui->plainTextEdit->toPlainText();
      scriptFile.flush();
      scriptFile.close();
//      QTextStream outputStream(&scriptFile);
//      QString code = m_MacroEditor->toPlainText();
//      code.replace("\n","\r\n");
//      outputStream << code;
//      scriptFile.close();
//      SetCurrentMacroFile( fileName );
//      SetMacroChanged( false );
    }
}

void MainWindow::on_actionTracks_triggered()
{
    dwidtracks = new DockWidgetTracks(this);
    dwidtracks->show();
    dwidtracks->resize(300,600);
    dwidtracks->pos();
    dwidtracks->repaint();
}

void MainWindow::on_pushButton_clicked()
{
    std::stringstream stream;
    string str;
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    unsigned int delta, track;
    unsigned long time;
    int messg;

    notes.clear(); // Clear notes list before adding new elements.
    tempos->clear(); // Clear tempos list before adding new elements.
    pitch_max = 1;
//    pitch_min = 100;


    for (int i = 0; getline(stream,str,'\t'); i++)
    {
//        int time, delta, track;
//        int messg;
        if (i == 0)
        {
            tpq = stoi(str.substr(5));
        }

        string messg_str;
        if (i >= 5 && i%4 == 0)
        {
            time = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 1)
        {
            delta = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 2)
        {
            track = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 3)
        {
            messg = stoi(str, nullptr, 16);
            messg_str = str;
            //ui->plainTextEdit->appendPlainText(QString::fromStdString(messg_str)); // this line is only for verification
        }
        //ui->plainTextEdit->appendPlainText(QString::fromStdString(str)); // this line is only for verification
        //ui->plainTextEdit->appendPlainText(QString::number(i)); // this line is only for verification

        // Save notes in notes list:
        if (i >= 5 && i%4 == 3) // If the current element of the plain text is a midi message
        {

            if (messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) > 0) // Condition for Note on is messg_str[0] == '9' and velocity > 0
            {
                MidiNote tempnote; // Create an object MidiNote
                tempnote.t_on = time;
                tempnote.pitch = stoi(messg_str.substr(3,2), nullptr, 16); // get the pitch from the midi message string
                tempnote.track = track;
                tempnote.vel = stoi(messg_str.substr(6,2), nullptr, 16); // get velocity from the midi message string
                notes.push_back(tempnote); // Insert the tempnote into the list "notes"
                if (tempnote.pitch > pitch_max)
                {
                        pitch_max = tempnote.pitch;
                        //cout << pitch_max << endl;
                }
                if (tempnote.pitch < pitch_min)
                        pitch_min = tempnote.pitch;
                if (tempnote.track > tracks_count)
                        tracks_count = tempnote.track;
            }
            if ((messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) == 0) || messg_str[0] == '8') // Condition for Note off is messg_str[0] == '9' and velocity == 0 or messg_str[0] == '8'
            {
                for (std::list<MidiNote>::iterator it=notes.end() ; it != notes.begin(); ) // note_off found: run the list backwards
                {
                    --it; // The pointer regress one position at the beginning, since the same message won't be on and off together.
                    if ((*it).track == track && (*it).pitch == (unsigned int)stoi(messg_str.substr(3,2), nullptr, 16)) // find the first note in the list with the same pitch and in the same track
                    {
                        (*it).t_off = time; // set its note_off time (the current time, since it has just found a note_off)
                        (*it).t_middle = (time + (*it).t_on)/2; // set its middle time ((t_on + t_off)/2)
                        break; // break this loop since the corresponding note_on to this current note_off was found!
                    }

                }
            }
            if (messg_str[0] == 'f' && messg_str[1] == 'f' && messg_str.size() >= 9) // Checks if it's a meta-message
            {
                // meta message is ill-formed.
                // meta messages must have at least three bytes:
                //    0: 0xff == meta message marker
                //    1: meta message type
                //    2: meta message data bytes to follow
                if (messg_str.substr(3,2) == "51" && messg_str.size() == 18) // Checks if it is a tempo change message
                {
                    // Meta tempo message can only be 6 bytes long.      ^ 18 is the string size: 6bytes*2hexDigits + 6 spaces (one at the end of each)
                    // The new_tempo calculated is given in microseconds per quarter note.
                    //ui->plainTextEdit->appendPlainText("MetaTempoMessage!\n"); // Line just for checking!
                    TempoChange temp_tempo_change; // Create a temporary object for tempo change
                    //temp_tempo_change.new_tempo = ((stoi(messg_str.substr(6,2), nullptr, 16) << 16) + (stoi(messg_str.substr(9,2), nullptr, 16) << 8) + stoi(messg_str.substr(12,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.new_tempo = ((stol(messg_str.substr(9,2), nullptr, 16) << 16) + (stol(messg_str.substr(12,2), nullptr, 16) << 8) + stol(messg_str.substr(15,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.t_on = time;
                    //cout << temp_tempo_change.new_tempo << "\n";
                    tempos->push_back(temp_tempo_change); // Stores the tempo change in the list
                }
            }

        }
    }
    total_time = time; // set the current time after processing as global variable total time from this midi processing.
    QMessageBox::information(this, tr("Processing completed"), "The midi data in the text input window was successfully completed.", QMessageBox::Ok );
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);


//    for (std::list<TempoChange>::iterator it=tempos.begin(); it != tempos.end(); ++it) // Run the  tempo change list forwards
//    {
//        ui->plainTextEdit->appendPlainText(QString::number((*it).new_tempo)); // just for checking if the tempos were stored in the list
//        ui->plainTextEdit->appendPlainText("\n");
//    }
}


void note_blocks_paint( cv::Mat image, char* window_name, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2;
    double x1, x2, y1, y2;
    //std::cout << "Paint blocks! " << pitch_min << ' ' << pitch_max << endl;
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        x1 = (double)window_width*((double)(*it).t_on/((double)endMidiTime - (double)startMidiTime));
        x2 = (double)window_width*((double)(*it).t_off/((double)endMidiTime - (double)startMidiTime));
        y1 = (double)window_height - (double)window_height*((double)(*it).pitch/((double)pitch_max - (double)pitch_min + 20.0));
        y2 = (double)window_height - (double)window_height*(((double)(*it).pitch + 1.0)/((double)pitch_max - (double)pitch_min + 20.0));
        pt1.x = (int)(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
        pt2.x = (int)(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
        pt1.y = (int)(y1); //window_height*((*it).pitch/(50));
        pt2.y = (int)(y2);//window_height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
        cv::rectangle( image, pt1, pt2, {(*it).vel,(*it).vel,(*it).vel}, 2, 8 );
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    int window_height = ui->spinBox_2->value();
    int window_width = ui->spinBox->value();

    cv::Mat *image_win1 = new cv::Mat;
    *image_win1 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    note_blocks_paint(*image_win1, "Note blocks", 0, total_time, window_width, window_height);

    cv::namedWindow("Note blocks");
    cv::imshow("Note blocks", *image_win1);

    cv::waitKey(0);
}



// Animation

void MainWindow::on_pushButton_3_clicked()
{
    int window_height = ui->spinBox_2->value();
    int window_width = ui->spinBox->value();


    cv::Mat *image_win2 = new cv::Mat;
    *image_win2 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    animwinP = new AnimwinP;

    cv::namedWindow("Animation");
    animbar = new AnimationBar(0, "Animation", image_win2, window_width, window_height, total_time, ui->doubleSpinBox->value(), tempos);
    animbar->show();

    animPt = new AnimPainter;

    if(ui->lineEdit->text().toStdString().size() > 4)
        video = new cv::VideoWriter(ui->lineEdit->text().toStdString(),CV_FOURCC(codec_fourcc->at(0),codec_fourcc->at(1),codec_fourcc->at(2),codec_fourcc->at(3)),ui->doubleSpinBox->value(), cv::Size(window_width,window_height),true); //CV_FOURCC('X','2','6','4')


    //animwinP->setup(100,200);

    //animbar->findChild<AnimationBar>("on_horizontalSlider_2_valueChanged");
    //animPt->blocks_paint(*image_win2, animwinP->zoom, animwinP->xpos, window_width, window_height);
    //cv::imshow("Animation", *image_win2);

    cv::waitKey(0);
}

//void on_trackbar1(int, void* )
//{
//  zoom = (double) zoom_slider/10 ;

//  note_blocks_paint(*image_win2, "Animation", xpos, (int)total_time/zoom_slider + xpos, ui->spinBox->value(), ui->spinBox_2->value());
//}
//void on_trackbar2(int, void* )
//{
//  xpos = (double) xpos_slider; //beta_slider_max ;

//  //cout << beta << endl;

//  //note_blocks_paint(img, "Animation", startMidiTime + xpos, (int)endMidiTime/zoom_slider + xpos, window_width, window_height);
//  note_blocks_paint(*image_win2, "Animation", xpos, (int)total_time/zoom_slider + xpos, ui->spinBox->value(), ui->spinBox_2->value());
//}

//void MainWindow::on_pushButton_3_clicked()
//{
//    int window_height = ui->spinBox_2->value();
//    int window_width = ui->spinBox->value();

//    cv::Mat *image_win2 = new cv::Mat;
//    *image_win2 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

//    char TrackbarNameA[20];
//    sprintf( TrackbarNameA, "Zoom %d", zoom_slider_max );
//    char TrackbarNameB[20];
//    sprintf( TrackbarNameB, "Position %d", xpos_slider_max );

//    cv::namedWindow("Animation", 1);
//    cv::createTrackbar( TrackbarNameA, "Animation", &zoom_slider, zoom_slider_max, on_trackbar1 );
//    cv::createTrackbar( TrackbarNameB, "Animation", &xpos_slider, xpos_slider_max, on_trackbar2 );
//    on_trackbar1(zoom_slider, 0 );
//    on_trackbar2(xpos_slider, 0 );
//}

void MainWindow::on_actionRendering_Properties_triggered()
{
    dwrenderprop = new DockWidRender(this);
    dwrenderprop->show();
}

void MainWindow::on_toolButton_clicked()
{
    QFileDialog *qvfsd = new QFileDialog; // qvfsd == Qt Video File Save Dialog
    ui->lineEdit->setText(qvfsd->getSaveFileName(this,
                                                 tr("Save video file"),
                                                 "//home//",
                                                 "Audio and Video Files (*.avi);;Moving Pictures Experts Group 4 (*.mp4 *.mpeg4);;Matroska Video File (*.mkv);;All Files (*.*)"));
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog *opentextfile;
    opentextfile = new QFileDialog();
    QString mvfileName = opentextfile->getOpenFileName(this,
                                                     tr("Open text file"),
                                                     "//home//",
                                                     "Música Visual File (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");
    QFile mvfile(mvfileName);
    if(!mvfile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", mvfile.errorString());
    }

    ui->plainTextEdit->clear();

    QTextStream in(&mvfile);
    //QString line = in.readLine();

    //ui->plainTextEdit->appendPlainText(in.readLine());

    while(!in.atEnd()) {
        QString line;// = in.readLine();
        ui->plainTextEdit->appendPlainText(in.readLine());
    }

    mvfile.close();
}
