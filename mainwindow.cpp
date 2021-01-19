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
#include "importmidi.h"
#include "Options.h"

// Include UI classes
#include "dockwidgettracks.h"
#include "animationbar.h"

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

//Include utils:
#include "utils/cnum.h"
#include "utils/colour.h"

// Iostream (just to help on debugging)
#include <iostream>

// Include fstream to save and open objects in .dat
#include <fstream>

// Include QDropEvent to be able to receive file drops
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

// include external functions
#include "renderChordStar.h"

using namespace std;

TracksP::TracksP()
{

}
RenderP::RenderP()
{

}

TracksP *tracksproperties = new TracksP(); // Global variable with tracks parameters for exhibition.
//int Mdt->NEvents; // Global var to store number of events
//std::list <MidiNote> Mdt->Notes; // List of processed notes.
//std::list <TempoChange> *Mdt->Tempos = new std::list <TempoChange>;
//std::list <TimeSignature> Mdt->TSignatures; // list of time signatures (being used for vertical lines)
//unsigned long mdt.TotalTime = 0; // Global variable to store the total time of the current processed midi file.
//unsigned int Mdt->PitchMax = 1, Mdt->PitchMin = 128;
//unsigned int Mdt->NTracks = 1;
//unsigned int Mdt->Tpq; // Ticks per Quarter-Note
//chords Mdt->GChords; // global var for chords

AnimationBar *animbar;
AnimwinP *animwinP;
AnimPainter *animPt;

RenderP *renderproperties = new RenderP();

//std::string videoFileName;
//bool *videoRecord = new bool(false);
//cv::VideoWriter *video;
//std::string *codec_fourcc = new std::string("X264");

//std::vector <std::string> *track_names; // store the track names when the button Process is pressed. // Redefined in tracksproperties, mainwindow.h
//std::vector<std::string> *Mdt->TrackNames = new std::vector<std::string>; // = {"Track 1", "Track 2", "Track 3", "Track 4", "Track 5", "Track 6", "Track 7", "Track 8", "Track 9", "Track 10", "Track 11", "Track 12", "Track 13", "Track 14", "Track 15", "Track 16", "Track 17", "Track 18", "Track 19", "Track 20", "Track 21", "Track 22", "Track 23", "Track 24"}; // this works, but is variable in size. So save/load settings won't work if put in TracksP.


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

void AnimPainter::blocks_paint(MusicData mdt, cv::Mat image, std::vector <cv::Mat> img_buffer_sep_tracks, int startMidiTime, int endMidiTime, int window_width, int window_height, VideoRecorder *vRec) // this function is called for every frame. startMidiTime is the time in the left side of the window, endMidiTime, at the right. These depend on playback position and zoom.
{
    int zoom = endMidiTime - startMidiTime;
    int curr_pos_middle = (startMidiTime + (zoom)/2);
    cv::Point pt1, pt2, pt3, pt4;//, pt5;// , pt6;
    cv::Point pt5[mdt.NTracks]; // for the interconnected lines. It has to be a vector of tracks_count length to make lines be connected only with notes within the same track.
    cv::Point pt1_prev[mdt.NTracks], pt2_prev[mdt.NTracks], pt3_prev[mdt.NTracks]; // these points are used for storing the previous respective points to make "moving notes" possible averaging current position/size with last position/size. There must be one for each track so that they are kept independent, i.e., moving notes in one track don't influence mov. notes in another (same idea as for interconn. lines).
    cv::Point ptos_prev[mdt.NTracks][4];
    int radius_prev[mdt.NTracks];// = 0;
    for (int i = 0; i < mdt.NTracks; i++) // start lines at zero. If not set, a rubish value may appear.
    {
        pt5[i].x = 0;
        pt5[i].y = 0;
    }
    float x1, x2, y1, y2, x3, y3;
    int x_max = window_width + 5000, y_max = window_height + 50;
    //int max_duration = window_width + 1000;
    //std::cout << "Paint blocks! " << Mdt->PitchMin << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    cv::Mat img_moving_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    //cv::Mat img_buffer_sep_tracks[tracks_count] = cv::Mat::zeros( window_height, window_width, CV_8UC3 ); // Qt 5.7, OpenCV 2.4 (which uses C++98)
    //cv::Mat img_buffer_sep_tracks[tracks_count] = {cv::Mat::zeros( window_height, window_width, CV_8UC3 )}; // this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11)
    // http://answers.opencv.org/question/31665/creating-an-array-of-mats-of-size-n/ - Static arrays need the size for their construction at compile time. If you want to have the size controlled at run-time, you need to either create the mat-array via new or use std::vector (I'd prefer the latter one, since when using new you'll also need to call delete[] and it also prevents you from writing at non existent memory)
    //std::vector <cv::Mat> img_buffer_sep_tracks(tracks_count, cv::Mat::zeros( window_height, window_width, CV_8UC3 ));// this does not solve for Qt 5.9, OpenCV 4.0 (which uses C++11), see comments below
    // now, if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // Solving this on the generation of the animation window
    cv::LineTypes lineType = renderproperties->shapeLineType;

    for (std::list<MidiNote>::iterator it=mdt.Notes.begin() ; it != mdt.Notes.end(); ++it) // Run the list forwards
    {
        if ((*it).is_note == 1 && startMidiTime -50 < (*it).t_off && endMidiTime + 50 > (*it).t_on) // is_note checks if it's a real note to avoid getting trash.
        {
            x1 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_on)/((float)endMidiTime - (float)startMidiTime)); // note_on time
            x2 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_off)/((float)endMidiTime - (float)startMidiTime)); // note_off time
            y1 = (float)window_height/2 - (float)window_height*((float)((*it).pitch - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
            y2 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 1.0 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
            pt1.x = f2int_safe(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
            pt2.x = f2int_safe(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
            pt1.y = f2int_safe(y1); //window_height*((*it).pitch/(50));
            pt2.y = f2int_safe(y2);//window_height*(((*it).pitch + 1)/(50));
            //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
            //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
            if (true) //(std::abs(pt1.x) < x_max && std::abs(pt1.y) < y_max && std::abs(pt2.x) < x_max && std::abs(pt2.y) < y_max);// && (*it).duration < max_duration) // This "if" is just to bypass an issue (certainly about big doubles being coverted to int), what causes random boxes appearing on the screen when one zooms in very close.
            {
                for (unsigned short tnum = 0; tnum < mdt.NTracks; tnum++)
                {
                    // -------------------------- Draw Interconnected Lines -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->interconnect[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (abs(pt3.x - pt5[tnum].x) < tracksproperties->max_connect_dist && pt5[tnum] != cv::Point(0,0))
                            if ( ! renderproperties->sep_render[1])
                                cv::line( image, pt3, pt5[tnum], {tracksproperties->getCv(tnum,2)/4,  tracksproperties->getCv(tnum,1)/4,  tracksproperties->getCv(tnum,0)/4}, 1, lineType );
                            else
                                cv::line( img_buffer_sep_tracks[tnum], pt3, pt5[tnum], {tracksproperties->getCv(tnum,2)/4,  tracksproperties->getCv(tnum,1)/4,  tracksproperties->getCv(tnum,0)/4}, 1, lineType );
                        pt5[tnum].x = f2int_safe(x3); // Center x of the previous note
                        pt5[tnum].y = f2int_safe(y3); // Center y of the previous note
                    }



                    // ------------------------- Draw Rectangles -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 0) // All tracks - rectangle
                    {
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::rectangle( img_playing_notes, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::rectangle( image, pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::rectangle( img_buffer_sep_tracks[tnum], pt1, pt2, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }


                    // -------------------------- Draw Rhombus -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 1) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType );
                    }


                    // -------------------------- Draw Ellipses -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 2) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Circles -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 3) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType );
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::circle( image, pt3, (x2-window_width/2+3), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                            else
                                cv::circle( img_playing_notes, pt3, (x2-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType );
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType );
                    }





                    // -------------------------- Draw Circles with moving centred circle -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 4) // All tracks
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);

                        if (pt1.x < window_width/2) // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


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
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::circle( img_playing_notes, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 2, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


                        if (pt1_prev[tnum].x <= window_width/2 && pt1.x > window_width/2 && pt2_prev[tnum].x + 5 > window_width/2) // The note block is inside the center line // no more else if
                        {
                            float deriv = (((float)window_width/2 - (float)pt1_prev[tnum].x)/((float)pt1.x - (float)pt1_prev[tnum].x));
                            int y_mov = (int) ((float)pt3_prev[tnum].y + deriv*((float)pt3.y - (float)pt3_prev[tnum].y));
                            int x_mov = window_width/2 + pt3_prev[tnum].x - pt1_prev[tnum].x + deriv*((pt3.x - pt1.x) - (pt3_prev[tnum].x - pt1_prev[tnum].x));
                            // Case 1: the distance to the next note is big (not a legato) -> note just fall off
                            if (pt1.x - pt2_prev[tnum].x > 5) // next_note t_on and old note t_off are far
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, pt3_prev[tnum], floor1(pt2_prev[tnum].x-window_width/2+1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::circle( image, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                                else
                                    cv::circle( img_moving_notes, cv::Point(x_mov, y_mov), floor1(radius_prev[tnum] + deriv*((float)radius - (float)radius_prev[tnum])), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);

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
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
//                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + pitch_max)/2)/((float)pitch_max - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
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
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        if (pt1.x <= window_width/2) // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
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
                                    cv::fillConvexPoly( image, ptos_prev[tnum], 4, {(0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (0.5 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_prev[tnum], 4, {(1 - deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, (1 - deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::fillConvexPoly( image, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                                else
                                    cv::fillConvexPoly( img_moving_notes, ptos_mov, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);

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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
//                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
//                        {
//                            if ( ! renderproperties->sep_render[0])
//                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                            else
//                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, 8 );
//                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


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
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (x3-x1) < 80 ? (x3-x1) : 80;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, 1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);


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
                                    cv::ellipse( image, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, pt3_prev[tnum], cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {(0.5-deriv)*getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, (0.5-deriv)*getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                            // Case 2: the end of the current note (prev) is close to the begin of the new note (legato) -> notes connect themselves
                            else
                                if ( ! renderproperties->sep_render[2])
                                    cv::ellipse( image, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);
                                else
                                    cv::ellipse( img_moving_notes, cv::Point(x3_mov, y3_mov), cv::Size(x3_mov-x1_mov, y3-y2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/64}, -1, lineType);

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
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        int radius = (*it).vel/2; // size proportion
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::circle( image, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::circle( img_playing_notes, pt3, 1.1*radius*(x2-window_width/2)/(x2-x1), {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::circle( image, pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::circle( img_buffer_sep_tracks[tnum], pt3, radius, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }



                    // -------------------------- Draw Ellipses with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 13) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt3.x = f2int_safe(x3); // Center x
                        pt3.y = f2int_safe(y3); // Center y
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, -1, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                            else
                                cv::ellipse( img_playing_notes, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, -1, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::ellipse( image, pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                            else
                                cv::ellipse( img_buffer_sep_tracks[tnum], pt3, cv::Size(x3-x1, 1 + (*it).vel/2 ), 0, 0, 360, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, 2, lineType);
                    }


                    // -------------------------- Draw Rhombus with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 14) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
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
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 4, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles V with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 15) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y1) + (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles ^ with height proportional to the velocity -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 16) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y3);
                        pt2.y = f2int_safe(y3);
                        pt3.x = f2int_safe(x3);
                        pt3.y = f2int_safe(y2) - (*it).vel/2;
                        cv::Point ptos[3];
                        ptos[0] = pt1; ptos[1] = pt3; ptos[2] = pt2;
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // -------------------------- Draw Triangles |> (forte-piano) -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 17) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt1.y); ptos[1] = pt3; ptos[2] = cv::Point(pt1.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = (float)(x2-window_width/2)/(x2-x1);
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }



                    // -------------------------- Draw Triangles <| (piano-forte) -------------------------
                    if ((*it).track == tnum && tracksproperties->active[tnum] == true && tracksproperties->shape[tnum] == 18) // All tracks - rectangle
                    {
                        x3 = (float)window_width*((-(float)startMidiTime + (float)(*it).t_middle)/((float)endMidiTime - (float)startMidiTime));
                        y3 = (float)window_height/2 - (float)window_height*((float)((*it).pitch + 0.5 - ((float)mdt.PitchMin + mdt.PitchMax)/2)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0 - renderproperties->vertShift;
                        pt1.y = f2int_safe(y1);
                        pt2.y = f2int_safe(y2);
                        pt3.x = f2int_safe(x2);
                        pt3.y = f2int_safe(y3);
                        cv::Point ptos[3];
                        ptos[0] = cv::Point(pt1.x, pt3.y); ptos[1] = cv::Point(pt2.x, pt1.y); ptos[2] = cv::Point(pt2.x, pt2.y);
                        if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                            if ( ! renderproperties->sep_render[1])
                                //cv::fillPoly( image, pts, );
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel*3/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel*3/512}, lineType);
                        else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                        {
                            float nprogress = 1.5 - (float)(x2-window_width/2)/(x2-x1);
                            if ( ! renderproperties->sep_render[0])
                                cv::fillConvexPoly( image, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/64, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/64}, lineType);
                            else
                                cv::fillConvexPoly( img_playing_notes, ptos, 3, {nprogress*getColorTrackP(tnum, (*it).pitch).b*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).g*(*it).vel/32, nprogress*getColorTrackP(tnum, (*it).pitch).r*(*it).vel/32}, lineType);
                        }
                        else // The note block is after (to the left of) the center line
                            if ( ! renderproperties->sep_render[1])
                                cv::fillConvexPoly( image, ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                            else
                                cv::fillConvexPoly( img_buffer_sep_tracks[tnum], ptos, 3, {getColorTrackP(tnum, (*it).pitch).b*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).g*(*it).vel/512, getColorTrackP(tnum, (*it).pitch).r*(*it).vel/512}, lineType);
                    }




                    // End shape drawings ------------------------------------------------


                    // ============= Vertical Lines from tracks =============

                    if ((*it).track == tnum && renderproperties->lines[3] && renderproperties->vlines_track_n == (int)tnum)
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
    if (renderproperties->lines[0]) // cetered line
        cv::line(image, cv::Point(window_width/2, window_height), cv::Point(window_width/2, 0), {100,100,100});
    if (renderproperties->lines[1]) // vertical lines from time signatures saved in the midi file
    {
        std::list<TimeSignature> tsigs = mdt.TSignatures; // for debugging
        TimeSignature tsig_next = TimeSignature(); // next physically, previous with reference to the backward loop:
        tsig_next.t_on = mdt.TotalTime + 100; // since TimeSignatures don't have a t_off time, we associate the total_time from the midi file to the very last time signature. The +100 is to force a last line t appear. +1 should already work, but this is still a mystery
        for (std::list<TimeSignature>::reverse_iterator ptsig = mdt.TSignatures.rbegin(); ptsig != mdt.TSignatures.rend(); ++ptsig) // run from last time signature to first
        {
            TimeSignature tsig = *ptsig;
            int k = 0; // for counting sub-beats and beats
            for (int i = tsig.t_on; i < tsig_next.t_on; i = i + 4*mdt.Tpq/tsig.denominator) // a sub-beat is 4*ticks_per_quarter_note/denominator
            {
                if (k % tsig.numerator == 0) // for a whole beat, we consider the numerator
                    cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {renderproperties->vlines_colour[2], renderproperties->vlines_colour[1], renderproperties->vlines_colour[0]});
                else
                    cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {0.6*renderproperties->vlines_colour[2], 0.6*renderproperties->vlines_colour[1], 0.6*renderproperties->vlines_colour[0]});
                k++;
            }
            tsig_next = tsig;
        }
    }
    if (renderproperties->lines[2]) // manual time signature, given measure
    {
        for (unsigned int i = 0; i < endMidiTime; i = i + 4*renderproperties->beat_measure_manual[0]*mdt.Tpq/renderproperties->beat_measure_manual[1])
        {
            cv::line(image, cv::Point((int)((float)window_width*((-(float)startMidiTime + (float)i)/((float)endMidiTime - (float)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {renderproperties->vlines_colour[2], renderproperties->vlines_colour[1], renderproperties->vlines_colour[0]});
        }
    }


// ==================== Horizontal Lines =============
    if (renderproperties->hlines)
    {
        float note_height = (float)window_height*((float)(1.0)/((float)mdt.PitchMax - (float)mdt.PitchMin))*renderproperties->vertRange/50.0; // this is the height of a note in pixels, i.e. the vertical space between 2 midi notes in the image
        int basePitchRef = mdt.PitchMax%12; // this is the reference for the pitch
        int pitch_span = mdt.PitchMax - mdt.PitchMin; // this is the whole pitch extent from the midi file
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



    // ============ Displaying note names ==============
    if (renderproperties->note_names && renderproperties->note_names_where == 0) // ToDo: create a new class for chord analysis, generate chord names, currently displaying only pitches
    {
        std::list<chordWithTime>::iterator it;
        std::list<chordWithTime>::iterator it_next;
        for (it = mdt.GChords.Chords.begin(), it_next = ++mdt.GChords.Chords.begin(); it_next!=(mdt.GChords.Chords.end()); ++it, ++it_next)
        {
            chordWithTime chordWT = *it;
            chordWithTime chordWT_next = *it_next;
            if (curr_pos_middle > chordWT_next.Start_time && (curr_pos_middle < chordWT.Start_time) && it!=mdt.GChords.Chords.begin() && it!=mdt.GChords.Chords.end())
            {
                std::string ptStr = "Pitches:";
                ptStr = chordWT.Chord.getPitchesStr(renderproperties->accidentalSharp);
                cv::putText(image,
                        ptStr,//"Here is some text",
                        cv::Point(10,30), // Coordinates
                        cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                        1.0, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1, // Line Thickness (Optional)
                        cv::LINE_AA); // Anti-alias (Optional)
            }

        }

    }

    // ============ Displaying chord names ============== ToDo

    // ============ Displaying circle / star ==============
    if (renderproperties->chord_star)
    {
        std::list<chordWithTime>::iterator it;
        std::list<chordWithTime>::iterator it_next;
        for (it = mdt.GChords.Chords.begin(), it_next = ++mdt.GChords.Chords.begin(); it_next!=(mdt.GChords.Chords.end()); ++it, ++it_next) // run through all chords
        {
            chordWithTime chordWT = *it;
            chordWithTime chordWT_next = *it_next;
            if (curr_pos_middle > chordWT_next.Start_time && (curr_pos_middle < chordWT.Start_time) && it!=mdt.GChords.Chords.begin() && it!=mdt.GChords.Chords.end()) // if it is the chord currently being played
            {
                int diam = 100;
                chord::circle type = renderproperties->chord_star_type;
                cv::Point centre = cv::Point(window_width/4, window_height/4);
                if ( ! (renderproperties->note_names_where == 1 && renderproperties->note_names))
                {
                    dispChordDisc(type, image, centre, diam, false, renderproperties->turn_chord_circle, renderproperties->accidentalSharp);
                }
                else if (renderproperties->note_names_where == 1 && renderproperties->note_names)
                {
                    dispChordDisc(type, image, centre, diam, true, renderproperties->turn_chord_circle, renderproperties->accidentalSharp);
                }
                chord currChord = chordWT.Chord;
                renderChordStar(currChord, type, image, centre, diam, renderproperties->turn_chord_circle);
            }

        }

    }

    // =========== Including separate layers ==========
    if (renderproperties->sep_render[0])
    {
        if (renderproperties->blur_size[0] > 0 && renderproperties->blur_size[1] > 0)
            cv::boxFilter(img_playing_notes, img_playing_notes, -1, cv::Size(renderproperties->blur_size[0], renderproperties->blur_size[1]));
        image = image + img_playing_notes;
    }
    if (renderproperties->sep_render[1])
    {
        for (short j = 0; j < (mdt.NTracks); j++)
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
    if (vRec->RecordVideo == 1 && vRec != nullptr)
    {
        vRec->writeFrame(image);
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
    this->Mdt = new MusicData(); // create object MusicData
    this->VRec = new VideoRecorder(720, 480, 30); // dimensions, fps etc. will be eventually changed later
}

MainWindow::~MainWindow()
{
    delete ui;
}

//================================================================================================

void MainWindow::ImportMidiFile(const char *midiFileName)
{
    ImportMidi impMid;
    impMid.importMidiMessagesText(midiFileName);

     ui->plainTextEdit->appendPlainText(QString::fromStdString(impMid.MidiMessages));
     Mdt->NEvents = impMid.NEvents;
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
    if (Mdt->TrackNames.size() < 24)
        Mdt->nameTracksReset();
    dwidtracks = new DockWidgetTracks(this, Mdt);
    dwidtracks->show();
    dwidtracks->resize(300,600);
    dwidtracks->pos();
    dwidtracks->repaint();
}

void MainWindow::on_pb_process_clicked() // Process button
{
    std::stringstream stream;
    string str;
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    str = stream.str();
    Mdt->processMidiString(str);
    QMessageBox::information(this, tr("Processing completed"), "The midi data in the text input window was successfully completed.", QMessageBox::Ok );
    ui->pb_noteBlocks->setEnabled(true);
    ui->pb_animation->setEnabled(true);
}


void note_blocks_paint( cv::Mat image, MusicData mdt, char* window_name, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2;
    double x1, x2, y1, y2;
    //std::cout << "Paint blocks! " << Mdt->PitchMin << ' ' << pitch_max << endl;
    for (std::list<MidiNote>::iterator it=mdt.Notes.begin() ; it != mdt.Notes.end(); ++it) // Run the list forwards
    {
        x1 = (double)window_width*((double)(*it).t_on/((double)endMidiTime - (double)startMidiTime));
        x2 = (double)window_width*((double)(*it).t_off/((double)endMidiTime - (double)startMidiTime));
        y1 = (double)window_height - (double)window_height*((double)(*it).pitch/((double)mdt.PitchMax - (double)mdt.PitchMin + 20.0));
        y2 = (double)window_height - (double)window_height*(((double)(*it).pitch + 1.0)/((double)mdt.PitchMax - (double)mdt.PitchMin + 20.0));
        pt1.x = (int)(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
        pt2.x = (int)(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
        pt1.y = (int)(y1); //window_height*((*it).pitch/(50));
        pt2.y = (int)(y2);//window_height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
        cv::rectangle( image, pt1, pt2, {(*it).vel,(*it).vel,(*it).vel}, 2, 8 );
    }
}

void MainWindow::on_pb_noteBlocks_clicked()
{
    int window_width = ui->spb_res_x->value();
    int window_height = ui->spb_res_y->value();

    cv::Mat *image_win1 = new cv::Mat;
    *image_win1 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    note_blocks_paint(*image_win1, *Mdt, "Note blocks", 0, Mdt->TotalTime, window_width, window_height);

    cv::namedWindow("Note blocks");
    cv::imshow("Note blocks", *image_win1);

    cv::waitKey(0);
}



// Animation

void MainWindow::on_pb_animation_clicked()
{
    int window_width = ui->spb_res_x->value();
    int window_height = ui->spb_res_y->value();


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
    for (int kz = 0; kz < Mdt->NTracks; kz++)
    {
        img_buffer_sep_tracks->push_back(mat_zeros.clone());
    }

    cv::namedWindow("Animation");
    animbar = new AnimationBar(0, (char*)"Animation", Mdt, image_win2, img_buffer_sep_tracks, window_width, window_height, ui->dsb_fps->value(), renderproperties, VRec);
    animbar->show();

    animPt = new AnimPainter;

    if(ui->edt_videoOutput->text().toStdString().size() > 0)
    {
        try
        {
            this->VRec->setVideoDim(window_width, window_height);
            this->VRec->Fps = ui->dsb_fps->value();
            this->VRec->OutputPath = ui->edt_videoOutput->text().toStdString();
            // CodecFourCC will be changed directly in object
            this->VRec->createVideoWriter();
            animbar->setRecButtonEnabled(true); // video should be ready to be written, activating button
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
    dwrenderprop = new DockWidRender(this, Mdt, VRec);
    dwrenderprop->show();
}

void MainWindow::on_toolButton_clicked()
{
    QFileDialog *qvfsd = new QFileDialog; // qvfsd == Qt Video File Save Dialog
    ui->edt_videoOutput->setText(qvfsd->getSaveFileName(this,
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
            dwidtracks = new DockWidgetTracks(this, Mdt);
            dwidtracks->show();
        }
        if (dwrenderprop != nullptr)
        {
            dwrenderprop->close();
            dwrenderprop = new DockWidRender(this, Mdt, VRec);
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
