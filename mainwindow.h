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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <algorithm>

#include "dockwidgettracks.h"
#include "dockwidrender.h"
#include "about.h"
#include "help1.h"

// Include lib classes
#include "midimessages.h"
#include "musicdata.h"
#include "visual/videorecorder.h"
#include "visual/tracks.h"

// From musiclib
#include "chords.h"

#include "opencv2/core.hpp"

class AnimwinP
{
public:
    int zoom;
    int xpos;
    int zoom_slider_max = 100;
    int xpos_slider_max = 200;
    int zoom_slider;
    int xpos_slider;
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
    void blocks_paint(MusicData mdt, cv::Mat, std::vector <cv::Mat>, int, int, int, int, TracksP, VideoRecorder*);
};
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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void ImportMidiFile(const char*);

    void on_actionImport_MIDI_File_triggered();

    void dragLeaveEvent(QDragLeaveEvent*);

    void dragMoveEvent(QDragMoveEvent*);

    void dragEnterEvent(QDragEnterEvent*);

    void dropEvent(QDropEvent*);

    void on_actionSave_as_triggered();

    void on_actionTracks_triggered();

    void on_actionRendering_Properties_triggered();

    void on_toolButton_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_settings_as_triggered();

    void on_actionLoad_settings_triggered();

    void on_actionAbout_triggered();

    void on_actionHow_does_it_work_triggered();

    void on_actionSqueeze_tracks_triggered();

    void on_pb_process_clicked();

    void on_pb_noteBlocks_clicked();

    void on_pb_animation_clicked();

private:
    Ui::MainWindow *ui;
    DockWidgetTracks *dwidtracks;
    DockWidRender *dwrenderprop = nullptr;
    About *aboutDiag = nullptr;
    Help1 *helpDiag_1 = nullptr;
    MusicData *Mdt = nullptr;
    VideoRecorder *VRec = nullptr;
    TracksP *TProp;
};

#endif // MAINWINDOW_H
