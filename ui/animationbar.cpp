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


#include "ui_animationbar.h"
#include "mainwindow.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "midimessages.h"
#include <string>

#include <iostream>

#include <QThread>

#include <QFileDialog>
//#include <QTimer>

//extern AnimwinP *animwinP;
//extern AnimPainter *animPt;
//extern std::list <TempoChange> *tempos;
//extern int mdt->Tpq;
//extern bool *videoRecord;
//extern cv::VideoWriter *video;

//cv::Mat *image;
//int window_width;
//int window_height;
//char* winName;
//int total_time;
//bool play = false;

//double current_time;

//QTimer *timerPlay;
//extern std::string *codec_fourcc;

AnimationBar::AnimationBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimationBar)
{
    ui->setupUi(this);
    // This original contructor is currently not used nor called. Use the below!
}

AnimationBar::AnimationBar(QWidget *parent, char* winName, MusicData *mdt, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, cv::Mat *playingNote, cv::Mat *movingNote, int window_width, int window_height, float fps, RenderP *rProp, std::list<LayerContainer> *layers, AnimPainter *aPainter, AnimState *aState, VideoRecorder *vRec):
    QWidget(parent),
    ui(new Ui::AnimationBar)
{
    ui->setupUi(this);
    this->Mdt = mdt;
    this->image = image;
    this->img_buffer_sep_tracks = img_buffer_sep_tracks;
    this->PlayingNote = playingNote;
    this->MovingNote = movingNote;
    this->window_width = window_width;
    this->window_height = window_height;
    this->winName = winName;
    this->fps = fps;
    this->RProp = rProp;
    this->Layers = layers;
    this->APainter = aPainter;
    this->AState = aState;
    this->VRec = vRec;
    ui->hSlider_zoom->setMaximum(mdt->TotalTime);
    ui->hSlider_playback->setMaximum(mdt->TotalTime);
    ui->spb_zoom->setMaximum(mdt->TotalTime);
    ui->spb_playback->setMaximum(mdt->TotalTime);

    ui->spb_zoom->setMinimum(mdt->Tpq);
    ui->hSlider_zoom->setMinimum(mdt->Tpq);

    ui->spb_zoom->setValue(64*mdt->Tpq);
    ui->hSlider_zoom->setValue(64*mdt->Tpq);
    ui->spb_playback->setValue(0);
    ui->hSlider_playback->setValue(0);

    ui->label_2->setText(QString::fromStdString(vRec->CodecFourCC));

    // create play thread, which will operate on the loop
    playThread = new PlayThread(this, fps);
    connect(playThread, SIGNAL(NumberChanged(int)), this, SLOT(onNumberChanged(int)));



    //DrawBlThread = new DrawBlocksThread(mdt, image, img_buffer_sep_tracks, window_width, window_height, fps, RProp, TProp, APainter, AState, VRec);
}

AnimationBar::~AnimationBar()
{
    // releasing memory before closing it in this destructor
    img_buffer_sep_tracks->clear();
    if (VRec->RecordVideo == true)
    {
        VRec->releaseVideo();
    }
    delete ui;
}


void AnimationBar::on_hSlider_zoom_valueChanged(int value)
{
    AState->setZoom(value);
    // create worker, which will operate on a single frame
    thread = new QThread();
    worker = new Worker(Mdt, image, img_buffer_sep_tracks, PlayingNote, MovingNote, window_width, window_height, APainter, AState, RProp, Layers, VRec, winName);
    worker->moveToThread(thread);
    //connect( worker, &Worker::error, this, &MyClass::errorString);
    connect( thread, &QThread::started, worker, &Worker::process);
    connect( worker, &Worker::finished, thread, &QThread::quit);
    connect( worker, &Worker::finished, worker, &Worker::deleteLater);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();

    if (RProp->extra_time[0] == 1)
    {
        ui->hSlider_playback->setMinimum(-value/2);
        ui->spb_playback->setMinimum(-value/2);
    }
    if (RProp->extra_time[1] == 1)
    {
        ui->hSlider_playback->setMaximum(Mdt->TotalTime+value/2);
        ui->spb_playback->setMaximum(Mdt->TotalTime+value/2);
    }
}
void AnimationBar::on_hSlider_playback_valueChanged(int value)
{
    AState->setXpos(value);
    thread = new QThread();
    worker = new Worker(Mdt, image, img_buffer_sep_tracks, PlayingNote, MovingNote, window_width, window_height, APainter, AState, RProp, Layers, VRec, winName);
    worker->moveToThread(thread);
    //connect( worker, &Worker::error, this, &MyClass::errorString);
    connect( thread, &QThread::started, worker, &Worker::process);
    connect( worker, &Worker::finished, thread, &QThread::quit);
    connect( worker, &Worker::finished, worker, &Worker::deleteLater);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    APainter->appendFrame(*image, VRec);
}

void AnimationBar::onNumberChanged(int num)
{
    //std::cout << num << ' ';
    std::list<TempoChange>::iterator it=Mdt->Tempos.end();
    it--; // This has to be done at the beginning to avoid trash!
    for (; it != Mdt->Tempos.begin(); --it) // Run the  tempo change list backwards
        if ((*it).t_on <= ui->spb_playback->value()) break; // If the current (spinBox_2) time is greater than the t_on from the tempo change, found the current tempo!
    double pace = ((double)Mdt->Tpq*1000000.0/(double)(*it).new_tempo/fps);// + 0.5); // The +0.5 is for correct rounding and the 192.0 should be the PPQ, but actually it can be other value.
    if (pace > 150) pace = 2; // improve this. The problem: the first new_tempo is wrongly read to be equal to tpq (pointer issue, I think)
    //std::cout << tpq << ' ' << (*it).new_tempo << ", start: " << (*it).t_on << "; size: " << tempos->size() << std::endl;
    AState->CurrentTime = AState->CurrentTime + pace;
    //ui->spinBox_2->setValue(ui->spinBox_2->value() + pace);// + 1);
    ui->spb_playback->setValue(AState->CurrentTime);// + 1);
    if (ui->spb_playback->value() == Mdt->TotalTime)
        playThread->stop = true;
    //playThread->wait(1); // wait with timeout for the next frame to respond
}

void AnimationBar::on_pb_play_clicked()
{
    playThread->stop = false;
    AState->CurrentTime = ui->spb_playback->value();
    playThread->start(QThread::LowPriority);
    //ui->spinBox_2->setValue(ui->spinBox_2->value() + 1000/30);
//    play = true;
//    //QThread::start(QThread::priority());
//    timerPlay = new QTimer;
//    while(play == true)
//    {
//        ui->spinBox_2->setValue(ui->spinBox_2->value() + 1000/30);
//        timerPlay->start(1000/30);
//        //QThread::wait(30);

//        //Parei aqui!!!
//    }
}

void AnimationBar::on_pb_pause_clicked()
{
    playThread->stop = true;
}

void AnimationBar::on_pb_rewind_clicked()
{
    ui->spb_playback->setValue(0);
    AState->CurrentTime = 0;
}

//void AnimationBar::on_pushButton_4_clicked()
//{
//    QFileDialog *qfsd = new QFileDialog;
//    *videoFileName = qfsd->getSaveFileName().toStdString();
//}

void AnimationBar::on_pb_recVideo_toggled(bool checked)
{
    VRec->RecordVideo = checked;
}

void AnimationBar::setRecButtonEnabled(bool value)
{
    ui->pb_recVideo->setEnabled(value);
}


Worker::Worker(MusicData *mdt, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, cv::Mat *playingNote, cv::Mat *movingNote, int winWidth, int winHeight, AnimPainter *aPainter, AnimState *aState, RenderP *rProp, std::list<LayerContainer> *layers, VideoRecorder *vRec, char* winName) { // Constructor
    this->Mdt = mdt;
    this->window_width = winWidth;
    this->window_height = winHeight;
    this->winName = winName;
    this->RProp = rProp;
    this->Layers = layers;
    this->APainter = aPainter;
    this->AState = aState;
    this->VRec = vRec;

    this->image = image;
    this->img_buffer_sep_tracks = img_buffer_sep_tracks;
    this->PlayingNote = playingNote;
    this->MovingNote = movingNote;
}

Worker::~Worker() { // Destructor
    // free resources
}

void Worker::process() { // Process. Start processing data.
    // allocate resources using new here
    //mutex.lock();

    *image = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    *PlayingNote = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    *MovingNote = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    AnimWindow aw;
    aw.StartMidiTime = AState->xpos - (AState->zoom)/2;
    aw.EndMidiTime = AState->xpos + (AState->zoom)/2;
    aw.Width = window_width;
    aw.Height = window_height;
    APainter->paintLayers(*Mdt, *image, *img_buffer_sep_tracks, *PlayingNote, *MovingNote, aw, *Layers, *RProp);
    cv::imshow(winName, *image);

    //mutex.unlock();

    emit finished();
}
