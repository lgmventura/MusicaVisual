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
#include <set>

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

// Include fstream to save and open objects in .dat
#include <fstream>

// Include QDropEvent to be able to receive file drops
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

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
unsigned int pitch_max = 1, pitch_min = 128;
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

//std::vector <std::string> *track_names; // store the track names when the button Process is pressed. // Redefined in tracksproperties, mainwindow.h
std::vector<std::string> *track_names = new std::vector<std::string>; // = {"Track 1", "Track 2", "Track 3", "Track 4", "Track 5", "Track 6", "Track 7", "Track 8", "Track 9", "Track 10", "Track 11", "Track 12", "Track 13", "Track 14", "Track 15", "Track 16", "Track 17", "Track 18", "Track 19", "Track 20", "Track 21", "Track 22", "Track 23", "Track 24"}; // this works, but is variable in size. So save/load settings won't work if put in TracksP.

short short_max = std::numeric_limits<short>::max();// = 32767;
short short_min = std::numeric_limits<short>::min();// = -32768;
short f2short_safe(float a)
{
    if (a > short_max)
        return short_max;
    else if ( a < short_min)
        return short_min;
    else
        return (short)(a + 0.5);
}
int int_max = std::numeric_limits<int>::max();// =  2147483647;
int int_min = std::numeric_limits<int>::min();// = -2147483648;
int f2int_safe(float a)
{
    if (a > int_max)
        return int_max;
    else if ( a < int_min)
        return int_min;
    else
        return (int)(a + 0.5);
}

int floor1(int a)
{
    if (a < 1)
        return 1;
    else
        return a;
}


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
        //hsvcolor.h = (double)((int)(hsvcolor.h + 180*pitch/12)%180);// - 180*(int)hsvcolor.h/180;
        hsvcolor.h = (double)((int)(hsvcolor.h + 360*pitch/12)%360);// - 180*(int)hsvcolor.h/180;
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
        //hsvcolor.h = (double)((int)(hsvcolor.h + 900*pitch/12)%180); // 180*5*pitch/12
        hsvcolor.h = (double)((int)(hsvcolor.h + 1800*pitch/12)%360); // 360*5*pitch/12
        rgbcolor = hsv2rgb(hsvcolor);
        rgbcolor.r = rgbcolor.r*255;
        rgbcolor.g = rgbcolor.g*255;
        rgbcolor.b = rgbcolor.b*255;
        return (rgbcolor);
    }

}

unsigned char hexval(unsigned char c) // https://stackoverflow.com/questions/3790613/how-to-convert-a-string-of-hex-values-to-a-string
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else abort();
}
void hex2ascii(const string& in, string& out) // https://stackoverflow.com/questions/3790613/how-to-convert-a-string-of-hex-values-to-a-string
{
    out.clear();
    out.reserve(in.length() / 2);
    for (string::const_iterator p = in.begin(); p != in.end(); p++)
    {
       unsigned char c = hexval(*p);
       p++;
       if (p == in.end()) break; // incomplete last digit - should report error
       c = (c << 4) + hexval(*p); // + takes precedence over <<
       out.push_back(c);
    }
}
void nameTracksReset()
{
    for (int i = 1; i <= 24; i++)
        track_names->push_back("Track " + std::to_string(i));
}

void AnimPainter::blocks_paint(cv::Mat image, std::vector <cv::Mat> img_buffer_sep_tracks, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2, pt3, pt4;//, pt5;// , pt6;
    cv::Point pt5[tracks_count]; // for the interconnected lines. It has to be a vector of tracks_count length to make lines be connected only with notes within the same track.
    cv::Point pt1_prev[tracks_count], pt2_prev[tracks_count], pt3_prev[tracks_count]; // these points are used for storing the previous respective points to make "moving notes" possible averaging current position/size with last position/size. There must be one for each track so that they are kept independent, i.e., moving notes in one track don't influence mov. notes in another (same idea as for interconn. lines).
    cv::Point ptos_prev[tracks_count][4];
    int radius_prev[tracks_count];// = 0;
    for (int i = 0; i < tracks_count; i++) // start lines at zero. If not set, a rubish value may appear.
    {
        pt5[i].x = 0;
        pt5[i].y = 0;
    }
    float x1, x2, y1, y2, x3, y3;
    int x_max = window_width + 5000, y_max = window_height + 50;
    //int max_duration = window_width + 1000;
    //std::cout << "Paint blocks! " << pitch_min << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    cv::Mat img_moving_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    //cv::Mat img_buffer_sep_tracks[tracks_count] = cv::Mat::zeros( window_height, window_width, CV_8UC3 ); // Qt 5.7, OpenCV 2.4 (which uses C++98)
    //cv::Mat img_buffer_sep_tracks[tracks_count] = {cv::Mat::zeros( window_height, window_width, CV_8UC3 )}; // this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11)
    // http://answers.opencv.org/question/31665/creating-an-array-of-mats-of-size-n/ - Static arrays need the size for their construction at compile time. If you want to have the size controlled at run-time, you need to either create the mat-array via new or use std::vector (I'd prefer the latter one, since when using new you'll also need to call delete[] and it also prevents you from writing at non existent memory)
    //std::vector <cv::Mat> img_buffer_sep_tracks(tracks_count, cv::Mat::zeros( window_height, window_width, CV_8UC3 ));// this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11), see comments below
    // now, if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // Solving this on the generation of the animation window

    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        if ((*it).is_note == 1 && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        {
            x1 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_on)/((float)endMidiTime - (float)startMidiTime)); // note_on time
            x2 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_off)/((float)endMidiTime - (float)startMidiTime)); // note_off time
            y1 = (float)window_height/2 - (float)window_height*((float)((*it).pitch - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
            y2 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 1.0 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
            pt1.x = f2int_safe(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
            pt2.x = f2int_safe(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
            pt1.y = f2int_safe(y1); //window_height*((*it).pitch/(50));
            pt2.y = f2int_safe(y2);//window_height*(((*it).pitch + 1)/(50));
            //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
            //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
            if (true) //(std::abs(pt1.x) < x_max && std::abs(pt1.y) < y_max && std::abs(pt2.x) < x_max && std::abs(pt2.y) < y_max);// && (*it).duration < max_duration) // This "if" is just to bypass an issue (certainly about big doubles being coverted to int), what causes random boxes appearing on the screen when one zooms in very close.
            {
                for (unsigned short tnum = 0; tnum < tracks_count; tnum++)
                {
                    // -------------------------- Draw Interconnected Lines -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->interconnect[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (abs(pt3.x - pt5[tnum].x) < tracksproperties->max_connect_dist && pt5[tnum] != cv::Point(0,0))
                            if ( ! renderproperties->sep_render[1])
                                cv::line( image, pt3, pt5[tnum], {tracksproperties->getCv(tnum,2)/4,  tracksproperties->getCv(tnum,1)/4,  tracksproperties->getCv(tnum,0)/4}, 1, 8 );
                            else
                                cv::line( img_buffer_sep_tracks[tnum], pt3, pt5[tnum], {tracksproperties->getCv(tnum,2)/4,  tracksproperties->getCv(tnum,1)/4,  tracksproperties->getCv(tnum,0)/4}, 1, 8 );
                        pt5[tnum].x = f2int_safe(x3); // Center x of the previous note
                        pt5[tnum].y = f2int_safe(y3); // Center y of the previous note
                    }



                    // ------------------------- Draw Rectangles -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 0) // All tracks - rectangle
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
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
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
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 2) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
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
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 3) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
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





                    // -------------------------- Draw Circles with moving centred circle -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 4) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );

                        if (pt1.x < window_width/2) // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                            // this section corresponds to the "moving notes", i.e., the notes being played morphing from the current to the last (or from the next to the current if you prefer to say).
                            // In general, it works averaging current position and size with the last ones. A very important detail is to store the previous points p1_prev, pt2_prev, ... in the
                            // corresponding track (so, pt1_prev[tnum], pt2_prev[tnum] etc.), because we don't want the very last note as previous note, but the previous note of the same track!
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }



                    // -------------------------- Draw Circles with moving centred circle and playing note highlight -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 5) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
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
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, 8 );
                            else
                                cv::circle( img_playing_notes, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 6) // All tracks
                    {
//                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
//                        pt3.x = f2int_safe(x3); // Center x
//                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );

                        else //if (pt1.x <= window_width/2) // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/256, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/256, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/256, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/256, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/256}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::rectangle( image, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //pt3_prev = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
//                        radius_prev = radius;
                    }



                    // -------------------------- Draw Rectangles with moving centred rectangle and highlighted borders -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 7) // All tracks
                    {
//                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
//                        pt3.x = f2int_safe(x3); // Center x
//                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                            else
                                cv::rectangle( img_playing_notes, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 2, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //int y_mov = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::rectangle( image, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(- 2 + pt1_prev[tnum].x + deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), 2 + pt1_prev[tnum].y + deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), cv::Point(2 + pt2_prev[tnum].x - deriv*(pt2_prev[tnum].x - pt1_prev[tnum].x), - 2 + pt2_prev[tnum].y - deriv*(pt2_prev[tnum].y - pt1_prev[tnum].y)), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::rectangle( image, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                                else
                                    cv::rectangle( img_moving_notes, cv::Point(x1_mov, y1_mov), cv::Point(x2_mov, y2_mov), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );

                        }

                        //pt3_prev = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
//                        radius_prev = radius;
                    }




                    // -------------------------- Draw Rhombus with moving rhombus -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 8) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
                        cv::Point ptos[4];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2; ptos[3] = pt4;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! renderproperties->sep_render[0])
//                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                            else
//                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                        }
                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            // int y2_mov = y1_mov;
                            int y3_mov = y1_mov + y1 - y3; // = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y4_mov = y1_mov - y1 + y3;
                            //
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            int x3_mov = window_width/2 + (pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x)))/2;
                            //int x3_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            // int x4_mov = x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(x1_mov, y1_mov); ptos_mov[1] = cv::Point(x3_mov, y3_mov); ptos_mov[2] = cv::Point(x2_mov, y1_mov); ptos_mov[3] = cv::Point(x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );

                        }
                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        ptos_prev[tnum][0] = ptos[0];
                        ptos_prev[tnum][1] = ptos[1];
                        ptos_prev[tnum][2] = ptos[2];
                        ptos_prev[tnum][3] = ptos[3];
                    }




                    // -------------------------- Draw Rhombus with moving rhombus and playing highlight (select sep playing notes render) -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 9) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1);
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2);
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
                        if (pt1.x <= window_width/2) // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! renderproperties->sep_render[0])
//                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                            else
//                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
//                        }
                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            //
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            // int y2_mov = y1_mov;
                            int y3_mov = y1_mov + y1 - y3; // = (int) ((float)pt3_prev.y + deriv*((float)pt3.y - (float)pt3_prev.y));
                            int y4_mov = y1_mov - y1 + y3;
                            //
                            int x1_mov = window_width/2;
                            int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            int x3_mov = window_width/2 + (pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x)))/2;
                            //int x3_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            // int x4_mov = x3_mov
                            cv::Point ptos_mov[4];
                            ptos_mov[0] = cv::Point(x1_mov, y1_mov); ptos_mov[1] = cv::Point(x3_mov, y3_mov); ptos_mov[2] = cv::Point(x2_mov, y1_mov); ptos_mov[3] = cv::Point(x3_mov, y4_mov);

                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );

                        }
                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        ptos_prev[tnum][0] = ptos[0];
                        ptos_prev[tnum][1] = ptos[1];
                        ptos_prev[tnum][2] = ptos[2];
                        ptos_prev[tnum][3] = ptos[3];
                    }



                    // -------------------------- Draw ellipses with moving centred ellipse -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 10) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! renderproperties->sep_render[0])
//                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                            else
//                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y3_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x3_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            //int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }




                    // -------------------------- Draw ellipses with moving centred ellipse and highlighted playing note -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 11) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, 8 );
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y3_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x3_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            int y1_mov = (int) ((float)pt1_prev[tnum].y + deriv*((float)pt1.y - (float)pt1_prev[tnum].y));
                            int y2_mov = (int) y1_mov + y2 - y1; // the difference among them remains the same
                            //int x_mov = window_width/2 + pt3_prev.x - pt1_prev.x + deriv*((pt3.x - pt1.x) - (pt3_prev.x - pt1_prev.x));
                            int x1_mov = window_width/2;
                            //int x2_mov = window_width/2 + pt2_prev[tnum].x - pt1_prev[tnum].x - deriv*((pt2_prev[tnum].x - pt1_prev[tnum].x) - (pt2.x - pt1.x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, 8 );

                        }

                        pt3_prev[tnum] = pt3; // Next centre point = previous
                        pt1_prev[tnum] = pt1;
                        pt2_prev[tnum] = pt2;
                        radius_prev[tnum] = radius;
                    }




                    // -------------------------- Draw Circles with size proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 12) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (*it).vel/2; // size proportion
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::circle( image, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            else
                                cv::circle( img_playing_notes, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                    }



                    // -------------------------- Draw Ellipses with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 13) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, 8 );
                    }


                    // -------------------------- Draw Rhombus with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 14) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1) + (*it).vel/2;
                        pt4.x = f2int_safe(x3);
                        pt4.y = f2int_safe(y2) - (*it).vel/2;
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



                    // -------------------------- Draw Triangles V with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 15) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1) + (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                    }



                    // -------------------------- Draw Triangles ^ with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 16) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y2) - (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                    }




                    // -------------------------- Draw Triangles |> (forte-piano) -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 17) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt1.y); ptos[1] = pt3; ptos[2] = cv::Point(pt1.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = (float)(x2-window_width/2)/(x2-x1);
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                    }



                    // -------------------------- Draw Triangles <| (piano-forte) -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 18) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)pitch_min + pitch_max)/2)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt3.y); ptos[1] = cv::Point(pt2.x, pt1.y); ptos[2] = cv::Point(pt2.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, 8 );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = 1.5 - (float)(x2-window_width/2)/(x2-x1);
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 8 );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, 8 );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 8 );
                    }




                    // End shape drawings ------------------------------------------------


                    // ============= Vertical Lines from tracks =============

                    if ((*it).track == tnum && renderproperties->lines[2] && renderproperties->vlines_track_n == (int)tnum)
                    {
                        cv::line(image, cv::Point(pt1.x, window_height), cv::Point(pt1.x, 0), {renderproperties->vlines_colour[2]*(*it).vel/128, renderproperties->vlines_colour[1]*(*it).vel/128, renderproperties->vlines_colour[0]*(*it).vel/128});
                    }
                }
            }
        }
    }
//    pt3.x = window_width/2;
//    pt4.x = window_width/2;
//    pt3.y = window_height;
//    pt4.y = 0;
// =================== Vertical Lines =============
    if (renderproperties->lines[0])
        cv::line(image, cv::Point(window_width/2, window_height), cv::Point(window_width/2, 0), {100,100,100});
    if (renderproperties->lines[1])
    {
        for (unsigned int i = 0; i < endMidiTime; i = i + 4*renderproperties->beat_measure_manual[0]*tpq/renderproperties->beat_measure_manual[1])
        {
            cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {renderproperties->vlines_colour[2], renderproperties->vlines_colour[1], renderproperties->vlines_colour[0]});
        }
    }


// ==================== Horizontal Lines =============
    if (renderproperties->hlines)
    {
        float note_height = (float)window_height*((float)(1.0)/((float)pitch_max - (float)pitch_min))*renderproperties->vertRange/50.0; // this is the height of a note in pixels, i.e. the vertical space between 2 midi notes in the image
        int basePitchRef = pitch_max%12; // this is the reference for the pitch
        int pitch_span = pitch_max - pitch_min; // this is the whole pitch extent from the midi file
        if(renderproperties->hlines_type == 0) // One line every n semitones + half shift
        {
            for (int i = 0; i < pitch_span/renderproperties->hlines_n + 1; i++)
            {
                int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i*renderproperties->hlines_n) - renderproperties->vertShift - renderproperties->hlines_offset*note_height + renderproperties->half_shift*note_height/2;
                hline_y = hline_y + basePitchRef*note_height;
                cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {renderproperties->hlines_colour[2], renderproperties->hlines_colour[1], renderproperties->hlines_colour[0]});
            }
        }
        if(renderproperties->hlines_type == 1) // One line every semitone and a brighter line every n semitones
        {
            for (int i = 0; i < pitch_span; i++)
            {
                if (i%renderproperties->hlines_n == basePitchRef)
                {
                   int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - renderproperties->vertShift - renderproperties->hlines_offset*note_height - renderproperties->half_shift*note_height/2;
                   cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {renderproperties->hlines_colour[2], renderproperties->hlines_colour[1], renderproperties->hlines_colour[0]});
                }
                else
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - renderproperties->vertShift - renderproperties->hlines_offset*note_height - renderproperties->half_shift*note_height/2;
                    cv::line(image, cv::Point(0, hline_y), cv::Point(window_width, hline_y), {renderproperties->hlines_colour[2]/2, renderproperties->hlines_colour[1]/2, renderproperties->hlines_colour[0]/2});
                }
            }
        }
        if(renderproperties->hlines_type == 2) // Keyboard
        {
            for (int i = 0; i < pitch_span + 1; i++)
            {
                short ii = (i - basePitchRef)%12;
                if (ii == 1 || ii == 3 || ii == 5 || ii == 8 || ii == 10)
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - renderproperties->vertShift - renderproperties->hlines_offset*note_height - renderproperties->half_shift*note_height/2;
                    cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(window_width, hline_y + note_height), {renderproperties->hlines_colour[2], renderproperties->hlines_colour[1], renderproperties->hlines_colour[0]}, -1);
                }
            }
        }
        if(renderproperties->hlines_type == 3) // Inverse Keyboard
        {
            for (int i = 0; i < pitch_span + 1; i++)
            {
                short ii = (i - basePitchRef)%12;
                if (ii == 0 || ii == 2 || ii == 4 || ii == 6 || ii == 7 || ii == 9 || ii == 11)
                {
                    int hline_y = (float)window_height/2 - note_height*(pitch_span)/2 + (note_height*i) - renderproperties->vertShift - renderproperties->hlines_offset*note_height - renderproperties->half_shift*note_height/2;
                    cv::rectangle(image, cv::Point(0, hline_y + 1), cv::Point(window_width, hline_y + note_height), {renderproperties->hlines_colour[2], renderproperties->hlines_colour[1], renderproperties->hlines_colour[0]}, -1);
                }
            }
        }
    }

    if (renderproperties->sep_render[0])
    {
        if (renderproperties->blur_size[0] > 0 && renderproperties->blur_size[1] > 0)
            cv::boxFilter(img_playing_notes, img_playing_notes, -1, cv::Size(renderproperties->blur_size[0], renderproperties->blur_size[1]));
        image = image + img_playing_notes;
    }
    if (renderproperties->sep_render[1])
    {
        for (short j = 0; j < (tracks_count); j++)
        {
            if (tracksproperties->track_blur[j] > 0)
                cv::boxFilter(img_buffer_sep_tracks[j], img_buffer_sep_tracks[j], -1, cv::Size(tracksproperties->track_blur[j], tracksproperties->track_blur[j]));
            image = image + img_buffer_sep_tracks[j];
            img_buffer_sep_tracks[j] = cv::Mat::zeros(window_height, window_width, CV_8UC3);
            //cv::add(image, img_buffer_sep_tracks[j], image); // this should do the same job as image = image + img_buffer_sep_tracks[j];

        }
    }
    //img_buffer_sep_tracks.clear();
    if (renderproperties->sep_render[2])
    {
        if (renderproperties->blur_size_movnotes[0] > 0 && renderproperties->blur_size_movnotes[1] > 0)
            cv::boxFilter(img_moving_notes, img_moving_notes, -1, cv::Size(renderproperties->blur_size_movnotes[0], renderproperties->blur_size_movnotes[1]));
        image = image + img_moving_notes;
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
    setAcceptDrops(true); // accept file droppings to ease the file opening
    dwidtracks = nullptr; // starting the variables as nullpointer because, if we click load_settings, it will attempt to reopen the widgets if they are open (see load_settings function). But if they had never been instantiated, MusicaVisual will crash.
    dwrenderprop = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//================================================================================================

void MainWindow::ImportMidiFile(const char *midiFileName)
{
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

void MainWindow::on_actionImport_MIDI_File_triggered()
{
    QString QmidiFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open MIDI File"),
                "//home//",
                "Midi Files (*.mid *.midi);;All Files (*.*)");

    QByteArray ba = QmidiFileName.toUtf8();
    const char *midiFileName = ba.data();

    MainWindow::ImportMidiFile(midiFileName);
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    //if (event->mimeData()->hasFormat("application/x-midi") || event->mimeData()->hasFormat("audio/midi") || event->mimeData()->hasFormat("audio/x-mid")  || event->mimeData()->hasFormat("audio/x-midi") || event->mimeData()->hasFormat("music/crescendo") || event->mimeData()->hasFormat("x-music/x-midi"))
    //{
    QString QmidiFileName;

        QList<QUrl> urls;
        QList<QUrl>::Iterator it;
        urls = event->mimeData()->urls();
        for (it = urls.begin(); it != urls.end(); it++)
            QmidiFileName = it->path();

        QByteArray ba = QmidiFileName.toUtf8();
        const char *midiFileName = ba.data();

        //ui->plainTextEdit->appendPlainText("Dropped midi");
        std::cout << "is midi\n" << midiFileName;
        //ui->plainTextEdit->appendPlainText(QmidiFileName);
        MainWindow::ImportMidiFile(midiFileName);

        event->acceptProposedAction();
    //}
}

void MainWindow::on_actionSave_as_triggered()
{
    QString QmvfFileName = QFileDialog::getSaveFileName(
                this,
                tr("Save MusicaVisual File"),
                "//home//",
                "Musica Visual (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");

    QByteArray ba = QmvfFileName.toUtf8();
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

void MainWindow::on_actionTracks_triggered() // open dockwidgettracks.
{
    if (track_names->size() < 24)
        nameTracksReset();
    dwidtracks = new DockWidgetTracks(this);
    dwidtracks->show();
    dwidtracks->resize(300,600);
    dwidtracks->pos();
    dwidtracks->repaint();
}

void MainWindow::on_pushButton_clicked() // Process button
{
    std::stringstream stream;
    string str;
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    unsigned int delta, track;
    unsigned long time;
    int messg;
    // reset track names, since we are processing a new midi file, to avoid garbage (old names, if you load a new midi file):
    nameTracksReset();

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
            tpq = stoi(str.substr(5)); // getting the tpq (ticks per quarter-note) on the first line
        }

        string messg_str;
        if (i >= 5 && i%4 == 0)
        {
            time = stoi(str, nullptr, 10); // getting the time in ticks
        }
        if (i >= 5 && i%4 == 1)
        {
            delta = stoi(str, nullptr, 10); // getting the delta time in ticks
        }
        if (i >= 5 && i%4 == 2)
        {
            track = stoi(str, nullptr, 10); // getting the track number
        }
        if (i >= 5 && i%4 == 3)
        {
            messg = stoi(str, nullptr, 16); // getting the midi message type
            messg_str = str; // getting the midi message string
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
                if (tempnote.track > tracks_count) // if there we see a track whose number is higher than the current number of tracks, we do number of tracks (aka tracks_count) = current track number
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
                        (*it).duration = time - (*it).t_on; // Sets its duration
                        (*it).is_note = 1; // This is a real note!
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
                // reading track names - Changing track names for easing instrument finding
                else if (messg_str.substr(3,2) == "3 ")// && messg_str.size() > 4) // Check if it is a track name meta message. I don't know its size, I put 4 bytes to test.
                {
                    std::string t_name;
                    std::string straux = messg_str.substr(9);
                    straux.erase(remove_if(straux.begin(), straux.end(), ::isspace), straux.end()); // remove_if is declared in <algorithm>
                    hex2ascii(straux, t_name);
                    if (track <= 24) // since we have a maximum of 24 tracks
                    {
                        track_names->at(track) = t_name; // store the track names when the button Process is pressed.
                    }
                }
            }

        }
    }
    tracks_count = tracks_count + 1; // This defines the final number of tracks detected in the processed midi. It is the highest track + 1, since the tracks are numbered from 0 to n-1.

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

    // now, if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // First of all, we need to declare the empty matrix:
    cv::Mat mat_zeros = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    // then we declare the vector of the same type, but without declaring its size yet:
    std::vector <cv::Mat> *img_buffer_sep_tracks = new std::vector <cv::Mat>;
    // and now we use the std::vector.push_back method to insert elements at the end of the vector, doing it for every track (so, I use the for). Then, inside this funcion, I clone the matrix.
    // the method clone() for cv::Mat creates a copy rather than an instance of the matrix. This is how I ensure I am giving having different matrices.
    for (int kz = 0; kz < tracks_count; kz++)
    {
        img_buffer_sep_tracks->push_back(mat_zeros.clone());
    }

    cv::namedWindow("Animation");
    animbar = new AnimationBar(0, "Animation", image_win2, img_buffer_sep_tracks, window_width, window_height, total_time, ui->doubleSpinBox->value(), tempos, renderproperties);
    animbar->show();

    animPt = new AnimPainter;

    if(ui->lineEdit->text().toStdString().size() > 0)
    {
        try
        {
            video = new cv::VideoWriter(ui->lineEdit->text().toStdString(),cv::VideoWriter::fourcc(codec_fourcc->at(0),codec_fourcc->at(1),codec_fourcc->at(2),codec_fourcc->at(3)),ui->doubleSpinBox->value(), cv::Size(window_width,window_height),true); //CV_FOURCC('X','2','6','4')
            //throw video = new cv::VideoWriter(ui->lineEdit->text().toStdString(),CV_FOURCC(codec_fourcc->at(0),codec_fourcc->at(1),codec_fourcc->at(2),codec_fourcc->at(3)),ui->doubleSpinBox->value(), cv::Size(window_width,window_height),true); //CV_FOURCC('X','2','6','4')
        }
        catch (cv::Exception& e)
        {
            QString msg = QString::fromStdString("Check if the video path and file name is valid and not read only. Check also if the selected fourcc codec (edit->render properties->video codec) is installed in your system. Perhaps you have to install ffmpeg or libavtools.");
            QMessageBox::critical(this, tr("Cannot write video"), msg, QMessageBox::Ok );
        }
    }

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

void MainWindow::on_actionSave_settings_as_triggered()
{
    QFileDialog *dsavesettings;
    dsavesettings = new QFileDialog;
    QString save_file_name = dsavesettings->getSaveFileName(this,
                                                           tr("Save settings"),
                                                           "//home//",
                                                           "Data file (*.dat);;Plain Text File (*.txt);;All Files (*.*)");
    ofstream output_file(save_file_name.toStdString(),ios::binary);
    output_file.write(reinterpret_cast<char*>(tracksproperties),sizeof(*tracksproperties));
    output_file.write(reinterpret_cast<char*>(renderproperties),sizeof(*renderproperties));
    output_file.close();
}

void MainWindow::on_actionLoad_settings_triggered()
{
    QFileDialog *dloadsettings;
    dloadsettings = new QFileDialog;
    QString open_file_name = dloadsettings->getOpenFileName(this,
                                                           tr("Load settings"),
                                                           "//home//",
                                                           "Data file (*.dat);;Plain Text File (*.txt);;All Files (*.*)");
    if (open_file_name != nullptr)
    {
        ifstream input_file(open_file_name.toStdString(),ios::binary);
        try
        {
            input_file.read(reinterpret_cast<char*>(tracksproperties),sizeof(*tracksproperties));
            input_file.read(reinterpret_cast<char*>(renderproperties),sizeof(*renderproperties));
            cout << "Settings file could be correctly interpreted.";
        }
        catch (exception& e)
        {
            QString msg = QString::fromStdString(std::strcat("Error on interpreting data from the file. C++ function: input_file.read(reinterpret_cast<char*>(data),sizeof(*data)); Description", e.what()));
            QMessageBox::critical(this, tr("Cannot load settings"), msg, QMessageBox::Ok );
        }
        catch (...)
        {
            QString msg = QString::fromStdString("Unexpected error on calling: input_file.read(reinterpret_cast<char*>(data),sizeof(*data));");
            QMessageBox::critical(this, tr("Cannot load settings"), msg, QMessageBox::Ok );
        }
        input_file.close();
        if (dwidtracks != nullptr)
        {
            dwidtracks->close();
            dwidtracks = new DockWidgetTracks(this);
            dwidtracks->show();
        }
        if (dwrenderprop != nullptr)
        {
            dwrenderprop->close();
            dwrenderprop = new DockWidRender(this);
            dwrenderprop->show();
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    //if (aboutDiag == nullptr)
    {
        aboutDiag = new About(this);
        aboutDiag->show();
    }
}

void MainWindow::on_actionHow_does_it_work_triggered()
{
    {
        helpDiag_1 = new Help1(this);
        helpDiag_1->show();
    }
}

//void print_set(aList) //only for debugging
//{
//    for (A const& a : aList)
//    {
//        std::cout << a << ' ';
//    }
//}

void MainWindow::on_actionSqueeze_tracks_triggered()
{
    std::stringstream stream; // for the whole text in the edit area
    string str; // one line
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    std::vector <unsigned int> tracks_list;

    std::set <unsigned short> tracks_set;
    std::set<unsigned short>::iterator it;
    std::pair<std::set<unsigned short>::iterator,bool> ret;

    unsigned short tnum_temp;
    std::string messg_str;
    //std::map <unsigned short, unsigned short> tracks_association;
    for (int i = 0; getline(stream,str,'\t'); i++)
    {
        if (i >= 5 && i%4 == 2) // position (column) of a track number in the plain text
        {
            tnum_temp = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 3) // getting midi message
        {
            //messg = stoi(str, nullptr, 16); // getting the midi message type
            messg_str = str; // getting the midi message string
            //ui->plainTextEdit->appendPlainText(QString::fromStdString(messg_str)); // this line is only for verification
        }
        if (messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) > 0) // checking midi message. Consider the track not empty if there is at least one note_on message.
        {
            tracks_list.push_back(tnum_temp); // getting the track number and inserting into the list
            ret = tracks_set.insert(tnum_temp); // getting the track number and inserting into the set. The set is like its mathematical definition, so it doesn't get repeated items
            //if (ret.second==false) // if no element was inserted into the set because the element is already there
            //    it=ret.first; // to improve inserting efficiency, see http://www.cplusplus.com/reference/set/set/insert/
        }
    }
    stream.str(std::string());
    stream.clear();
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box. Run it again for replacing track numbers.
    std::stringstream stream2;
    std::string str2;

    ui->plainTextEdit->clear();
    //ui->plainTextEdit->appendPlainText(QString::fromStdString(std::to_string(tracks_set.size()))); // only for debug
    for (int i = 0; getline(stream,str,'\t'); i++)
    {
        if (i < 5) // for the first lines
            str2.append(str); // nothing changes
        else if (i >= 5 && i%4 != 2) // for the following changes, if it is not in the track column,
            str2.append(str); // nothing changes
        else //if (i >= 5 && i%4 == 2) // else
        {
            it = tracks_set.find(stoi(str, nullptr, 10)); // find position of the track in the new set containing only non-empty tracks
            str2.append(std::to_string(std::distance(tracks_set.begin(), it))); // insert it to the auxiliar string str2
        }
        stream2 << str2;
        stream2 << '\t';

        str2.clear();
    }
    ui->plainTextEdit->appendPlainText(QString::fromStdString(stream2.str()));

//    ui->plainTextEdit->appendPlainText(QString::fromStdString(std::to_string(tracks_set.size())));
//    for(it = tracks_set.begin(); it != tracks_set.end(); it++)
//        {
//            ui->plainTextEdit->appendPlainText(QString::fromStdString(std::to_string(*it)));// << '\n';
//            ui->plainTextEdit->appendPlainText(QString::fromStdString("  "));
//            //outstream_1 << std::to_string(*it) << '\n';
//            //str = std::to_string(*it);
//        }
    tracks_list.clear();
    tracks_set.clear();
}
