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

#include "animationbar.h"
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

extern AnimwinP *animwinP;
extern AnimPainter *animPt;
//extern std::list <TempoChange> *tempos;
extern int tpq;
extern bool *videoRecord;
extern cv::VideoWriter *video;

cv::Mat *image;
int window_width;
int window_height;
char* winName;
//int total_time;
bool play = false;

double current_time;

//QTimer *timerPlay;
extern std::string *codec_fourcc;

AnimationBar::AnimationBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimationBar)
{
    ui->setupUi(this);
    // This original contructor is currently not used nor called. Use the below!
}

AnimationBar::AnimationBar(QWidget *parent, char* winName, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, int window_width, int window_height, int total_time, float fps, std::list <TempoChange> *tempos, RenderP *renderproperties):
    QWidget(parent),
    ui(new Ui::AnimationBar)
{
    ui->setupUi(this);
    this->image = image;
    this->img_buffer_sep_tracks = img_buffer_sep_tracks;
    this->window_width = window_width;
    this->window_height = window_height;
    this->winName = winName;
    this->total_time;
    this->fps = fps;
    this->tempos = tempos;
    this->renderproperties = renderproperties;
    this->total_time = total_time;
    ui->horizontalSlider->setMaximum(total_time);
    ui->horizontalSlider_2->setMaximum(total_time);
    ui->spinBox->setMaximum(total_time);
    ui->spinBox_2->setMaximum(total_time);

    ui->spinBox->setMinimum(tpq);
    ui->horizontalSlider->setMinimum(tpq);

    ui->spinBox->setValue(64*tpq);
    ui->horizontalSlider->setValue(64*tpq);
    ui->spinBox_2->setValue(0);
    ui->horizontalSlider_2->setValue(0);

    ui->label_2->setText(QString::fromStdString(*codec_fourcc));

    playThread = new PlayThread(this, fps);
    connect(playThread, SIGNAL(NumberChanged(int)), this, SLOT(onNumberChanged(int)));
}

AnimationBar::~AnimationBar()
{
    // releasing memory before closing it in this destructor
    img_buffer_sep_tracks->clear();
    if (*videoRecord == true)
    {
        video->release();
    }
    delete ui;
}


void AnimationBar::on_horizontalSlider_valueChanged(int value)
{
    animwinP->setZoom(value);
    *image = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    animPt->blocks_paint(*image, *img_buffer_sep_tracks, animwinP->xpos - (animwinP->zoom)/2, animwinP->xpos + (animwinP->zoom)/2, window_width, window_height);
    cv::imshow(winName, *image);

    if (renderproperties->extra_time[0] == 1)
    {
        ui->horizontalSlider_2->setMinimum(-value/2);
        ui->spinBox_2->setMinimum(-value/2);
    }
    if (renderproperties->extra_time[1] == 1)
    {
        ui->horizontalSlider_2->setMaximum(total_time+value/2);
        ui->spinBox_2->setMaximum(total_time+value/2);
    }
}
void AnimationBar::on_horizontalSlider_2_valueChanged(int value)
{
    animwinP->setXpos(value);
    *image = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    animPt->blocks_paint(*image, *img_buffer_sep_tracks, animwinP->xpos - (animwinP->zoom)/2, animwinP->xpos + (animwinP->zoom)/2, window_width, window_height);
    cv::imshow(winName, *image);
}

void AnimationBar::onNumberChanged(int num)
{
    //std::cout << num << ' ';
    std::list<TempoChange>::iterator it=tempos->end();
    it--; // This has to be done at the beginning to avoid trash!
    for (; it != tempos->begin(); --it) // Run the  tempo change list backwards
        if ((*it).t_on <= ui->spinBox_2->value()) break; // If the current (spinBox_2) time is greater than the t_on from the tempo change, found the current tempo!
    double pace = ((double)tpq*1000000.0/(double)(*it).new_tempo/fps);// + 0.5); // The +0.5 is for correct rounding and the 192.0 should be the PPQ, but actually it can be other value.
    if (pace > 150) pace = 2; // improve this. The problem: the first new_tempo is wrongly read to be equal to tpq (pointer issue, I think)
    //std::cout << tpq << ' ' << (*it).new_tempo << ", start: " << (*it).t_on << "; size: " << tempos->size() << std::endl;
    current_time = current_time + pace;
    //ui->spinBox_2->setValue(ui->spinBox_2->value() + pace);// + 1);
    ui->spinBox_2->setValue(current_time);// + 1);
    if (ui->spinBox_2->value() == total_time)
        playThread->stop = true;
}

void AnimationBar::on_pushButton_2_clicked()
{
    playThread->stop = false;
    current_time = ui->spinBox_2->value();
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

void AnimationBar::on_pushButton_3_clicked()
{
    playThread->stop = true;
}

void AnimationBar::on_pushButton_clicked()
{
    ui->spinBox_2->setValue(0);
    current_time = 0;
}

//void AnimationBar::on_pushButton_4_clicked()
//{
//    QFileDialog *qfsd = new QFileDialog;
//    *videoFileName = qfsd->getSaveFileName().toStdString();
//}

void AnimationBar::on_pushButton_4_toggled(bool checked)
{
    *videoRecord = checked;
}

void AnimationBar::setRecButtonEnabled(bool value)
{
    ui->pushButton_4->setEnabled(value);
}
