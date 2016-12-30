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

#include "dockwidgettracks.h"
#include "dockwidrender.h"
#include "about.h"
#include "help1.h"

#include "opencv2/core.hpp"

class TracksP
{
public:
    TracksP();
    int tColours[16][3]= {
        {0, 0, 200} ,   //1 (index 0)
        {0, 120, 120} ,   //2
        {0, 200, 0},    //3
        {120, 120, 0},    //4
        {200, 0, 0},    //5
        {120, 0, 120},    //6
        {80, 80, 200} ,   //7
        {0, 200, 200} , //8
        {80, 200, 80},    //9
        {100, 100, 100},//10
        {100, 100, 0},  //11
        {200, 0, 0},    //12
        {100, 0, 100},  //13
        {50, 100, 150}, //14
        {150, 100, 50}, //15
        {100, 200, 100} //16 (index 15)
    };
    bool active[16] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    short shape[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    short track_blur[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    short interconnect[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    int max_connect_dist = 100;
    short colorScheme[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    //int getColour()
    int getCv(int a, int b) {return (tColours[a][b]);}
    void setCv(int a, int b, int v) {tColours[a][b] = v;}
};
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
    int zoom;
    int xpos;
    int win_width;
    int win_height;
    void blocks_paint(cv::Mat, int, int, int, int);
};
class RenderP
{
public:
    RenderP();
    int vertRange = 40;
    int vertShift = 0;
    bool lines[2] = {1,0};
    unsigned short beat_measure_manual[2] = {4,4};
    bool sep_render[3] = {0,0,0};
    short blur_size[2] = {0,0};
    short blur_size_movnotes[2] = {0,0};
    bool extra_time[2] = {0,1};
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

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_actionRendering_Properties_triggered();

    void on_toolButton_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_settings_as_triggered();

    void on_actionLoad_settings_triggered();

    void on_actionAbout_triggered();

    void on_actionHow_does_it_work_triggered();

private:
    Ui::MainWindow *ui;
    DockWidgetTracks *dwidtracks;
    DockWidRender *dwrenderprop = nullptr;
    About *aboutDiag = nullptr;
    Help1 *helpDiag_1 = nullptr;
};

#endif // MAINWINDOW_H
