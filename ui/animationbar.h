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

#ifndef ANIMATIONBAR_H
#define ANIMATIONBAR_H

#include <QWidget>

#include "opencv2/core.hpp"

#include "playthread.h"

//#include "mainwindow.h" // now mainwindow includes this animationbar.h. There cannot be a cycle of includes, otherwise the compiler seems to ignore the last one and generates an error that the class
//wasn't defined, as if we hadn't included the this file, actually.
#include "musiclib/musicdata.h"
#include "visual/videorecorder.h"
#include "trackssetup.h"
#include "rendersetup.h"
#include "midimessages.h"

namespace Ui {
class AnimationBar;
}

class AnimationBar : public QWidget
{
    Q_OBJECT

public:
    explicit AnimationBar(QWidget *parent = 0);
    MusicData *Mdt;
    VideoRecorder *VRec;
    RenderP *RProp;
    TracksP *TProp;
    AnimPainter *APainter;
    AnimState *AState;
    cv::Mat *image;
    std::vector <cv::Mat> *img_buffer_sep_tracks;
    int window_height;
    int window_width;
    char *winName;
    int total_time;
    float fps;

    PlayThread *playThread;

    AnimationBar(QWidget *parent, char* winName, MusicData *mdt, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, int window_width, int window_height, float fps, RenderP *rProp, TracksP *tProp, AnimPainter *aPainter, AnimState *aState, VideoRecorder *vRec = nullptr);

    ~AnimationBar();

private slots:
    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

//    void on_pushButton_4_clicked();

    void on_pushButton_4_toggled(bool checked);

public slots:
    void onNumberChanged(int num);

    void setRecButtonEnabled(bool value);

private:
    Ui::AnimationBar *ui;
};

#endif // ANIMATIONBAR_H
