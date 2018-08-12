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

#include "dockwidgettracks.h"
#include "ui_dockwidgettracks.h"

#include "mainwindow.h"
#include "dialogcolorpicker.h"

#include <QColor>
#include <QColorDialog>

//TracksP *tracksproperties = new TracksP();
extern TracksP *tracksproperties;
extern std::vector <std::string> *track_names;

DockWidgetTracks::DockWidgetTracks(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidgetTracks)
{
    ui->setupUi(this);

    ct1.setRgb(tracksproperties->getCv(0,0), tracksproperties->getCv(0,1), tracksproperties->getCv(0,2));
    QPalette pal = ui->widget1->palette();
    pal.setColor(QPalette::Window, ct1);
    ui->widget1->setPalette(pal);

    ct2.setRgb(tracksproperties->getCv(1,0), tracksproperties->getCv(1,1), tracksproperties->getCv(1,2));
    QPalette pal2 = ui->widget1_2->palette();
    pal2.setColor(QPalette::Window, ct2);
    ui->widget1_2->setPalette(pal2);

    ct3.setRgb(tracksproperties->getCv(2,0), tracksproperties->getCv(2,1), tracksproperties->getCv(2,2));
    QPalette pal3 = ui->widget1_3->palette();
    pal3.setColor(QPalette::Window, ct3);
    ui->widget1_3->setPalette(pal3);

    ct4.setRgb(tracksproperties->getCv(3,0), tracksproperties->getCv(3,1), tracksproperties->getCv(3,2));
    QPalette pal4 = ui->widget1_4->palette();
    pal4.setColor(QPalette::Window, ct4);
    ui->widget1_4->setPalette(pal4);

    ct5.setRgb(tracksproperties->getCv(4,0), tracksproperties->getCv(4,1), tracksproperties->getCv(4,2));
    QPalette pal5 = ui->widget1_5->palette();
    pal5.setColor(QPalette::Window, ct5);
    ui->widget1_5->setPalette(pal5);

    ct6.setRgb(tracksproperties->getCv(5,0), tracksproperties->getCv(5,1), tracksproperties->getCv(5,2));
    QPalette pal6 = ui->widget1_6->palette();
    pal6.setColor(QPalette::Window, ct6);
    ui->widget1_6->setPalette(pal6);

    ct7.setRgb(tracksproperties->getCv(6,0), tracksproperties->getCv(6,1), tracksproperties->getCv(6,2));
    QPalette pal7 = ui->widget1_7->palette();
    pal7.setColor(QPalette::Window, ct7);
    ui->widget1_7->setPalette(pal7);

    ct8.setRgb(tracksproperties->getCv(7,0), tracksproperties->getCv(7,1), tracksproperties->getCv(7,2));
    QPalette pal8 = ui->widget1_8->palette();
    pal8.setColor(QPalette::Window, ct8);
    ui->widget1_8->setPalette(pal8);

    ct9.setRgb(tracksproperties->getCv(8,0), tracksproperties->getCv(8,1), tracksproperties->getCv(8,2));
    QPalette pal9 = ui->widget1_9->palette();
    pal9.setColor(QPalette::Window, ct9);
    ui->widget1_9->setPalette(pal9);

    ct10.setRgb(tracksproperties->getCv(9,0), tracksproperties->getCv(9,1), tracksproperties->getCv(9,2));
    QPalette pal10 = ui->widget1_10->palette();
    pal10.setColor(QPalette::Window, ct10);
    ui->widget1_10->setPalette(pal10);

    ct11.setRgb(tracksproperties->getCv(10,0), tracksproperties->getCv(10,1), tracksproperties->getCv(10,2));
    QPalette pal11 = ui->widget1_11->palette();
    pal11.setColor(QPalette::Window, ct11);
    ui->widget1_11->setPalette(pal11);

    ct12.setRgb(tracksproperties->getCv(11,0), tracksproperties->getCv(11,1), tracksproperties->getCv(11,2));
    QPalette pal12 = ui->widget1_12->palette();
    pal12.setColor(QPalette::Window, ct12);
    ui->widget1_12->setPalette(pal12);

    ct13.setRgb(tracksproperties->getCv(12,0), tracksproperties->getCv(12,1), tracksproperties->getCv(12,2));
    QPalette pal13 = ui->widget1_13->palette();
    pal13.setColor(QPalette::Window, ct13);
    ui->widget1_13->setPalette(pal13);

    ct14.setRgb(tracksproperties->getCv(13,0), tracksproperties->getCv(13,1), tracksproperties->getCv(13,2));
    QPalette pal14 = ui->widget1_14->palette();
    pal14.setColor(QPalette::Window, ct14);
    ui->widget1_14->setPalette(pal14);

    ct15.setRgb(tracksproperties->getCv(14,0), tracksproperties->getCv(14,1), tracksproperties->getCv(14,2));
    QPalette pal15 = ui->widget1_15->palette();
    pal15.setColor(QPalette::Window, ct15);
    ui->widget1_15->setPalette(pal15);

    ct16.setRgb(tracksproperties->getCv(15,0), tracksproperties->getCv(15,1), tracksproperties->getCv(15,2));
    QPalette pal16 = ui->widget1_16->palette();
    pal16.setColor(QPalette::Window, ct16);
    ui->widget1_16->setPalette(pal16);

    ct17.setRgb(tracksproperties->getCv(16,0), tracksproperties->getCv(16,1), tracksproperties->getCv(16,2));
    QPalette pal17 = ui->widget1_17->palette();
    pal17.setColor(QPalette::Window, ct17);
    ui->widget1_17->setPalette(pal17);

    ct18.setRgb(tracksproperties->getCv(17,0), tracksproperties->getCv(17,1), tracksproperties->getCv(17,2));
    QPalette pal18 = ui->widget1_18->palette();
    pal18.setColor(QPalette::Window, ct18);
    ui->widget1_18->setPalette(pal18);

    ct19.setRgb(tracksproperties->getCv(18,0), tracksproperties->getCv(18,1), tracksproperties->getCv(18,2));
    QPalette pal19= ui->widget1_19->palette();
    pal19.setColor(QPalette::Window, ct19);
    ui->widget1_19->setPalette(pal19);

    ct20.setRgb(tracksproperties->getCv(19,0), tracksproperties->getCv(19,1), tracksproperties->getCv(19,2));
    QPalette pal20= ui->widget1_20->palette();
    pal20.setColor(QPalette::Window, ct20);
    ui->widget1_20->setPalette(pal20);

    ct21.setRgb(tracksproperties->getCv(20,0), tracksproperties->getCv(20,1), tracksproperties->getCv(20,2));
    QPalette pal21= ui->widget1_21->palette();
    pal21.setColor(QPalette::Window, ct21);
    ui->widget1_21->setPalette(pal21);

    ct22.setRgb(tracksproperties->getCv(21,0), tracksproperties->getCv(21,1), tracksproperties->getCv(21,2));
    QPalette pal22= ui->widget1_22->palette();
    pal22.setColor(QPalette::Window, ct22);
    ui->widget1_22->setPalette(pal22);

    ct23.setRgb(tracksproperties->getCv(22,0), tracksproperties->getCv(22,1), tracksproperties->getCv(22,2));
    QPalette pal23= ui->widget1_23->palette();
    pal23.setColor(QPalette::Window, ct23);
    ui->widget1_23->setPalette(pal23);

    ct24.setRgb(tracksproperties->getCv(23,0), tracksproperties->getCv(23,1), tracksproperties->getCv(23,2));
    QPalette pal24= ui->widget1_24->palette();
    pal24.setColor(QPalette::Window, ct24);
    ui->widget1_24->setPalette(pal24);



    ui->notesAppear1->setCurrentIndex(tracksproperties->shape[0]);
    ui->notesAppear1_2->setCurrentIndex(tracksproperties->shape[1]);
    ui->notesAppear1_3->setCurrentIndex(tracksproperties->shape[2]);
    ui->notesAppear1_4->setCurrentIndex(tracksproperties->shape[3]);
    ui->notesAppear1_5->setCurrentIndex(tracksproperties->shape[4]);
    ui->notesAppear1_6->setCurrentIndex(tracksproperties->shape[5]);
    ui->notesAppear1_7->setCurrentIndex(tracksproperties->shape[6]);
    ui->notesAppear1_8->setCurrentIndex(tracksproperties->shape[7]);
    ui->notesAppear1_9->setCurrentIndex(tracksproperties->shape[8]);
    ui->notesAppear1_10->setCurrentIndex(tracksproperties->shape[9]);
    ui->notesAppear1_11->setCurrentIndex(tracksproperties->shape[10]);
    ui->notesAppear1_12->setCurrentIndex(tracksproperties->shape[11]);
    ui->notesAppear1_13->setCurrentIndex(tracksproperties->shape[12]);
    ui->notesAppear1_14->setCurrentIndex(tracksproperties->shape[13]);
    ui->notesAppear1_15->setCurrentIndex(tracksproperties->shape[14]);
    ui->notesAppear1_16->setCurrentIndex(tracksproperties->shape[15]);
    ui->notesAppear1_17->setCurrentIndex(tracksproperties->shape[16]);
    ui->notesAppear1_18->setCurrentIndex(tracksproperties->shape[17]);
    ui->notesAppear1_19->setCurrentIndex(tracksproperties->shape[18]);
    ui->notesAppear1_20->setCurrentIndex(tracksproperties->shape[19]);
    ui->notesAppear1_21->setCurrentIndex(tracksproperties->shape[20]);
    ui->notesAppear1_22->setCurrentIndex(tracksproperties->shape[21]);
    ui->notesAppear1_23->setCurrentIndex(tracksproperties->shape[22]);
    ui->notesAppear1_24->setCurrentIndex(tracksproperties->shape[23]);

    ui->spinBox->setValue(tracksproperties->track_blur[0]);
    ui->spinBox_2->setValue(tracksproperties->track_blur[1]);
    ui->spinBox_3->setValue(tracksproperties->track_blur[2]);
    ui->spinBox_4->setValue(tracksproperties->track_blur[3]);
    ui->spinBox_5->setValue(tracksproperties->track_blur[4]);
    ui->spinBox_6->setValue(tracksproperties->track_blur[5]);
    ui->spinBox_7->setValue(tracksproperties->track_blur[6]);
    ui->spinBox_8->setValue(tracksproperties->track_blur[7]);
    ui->spinBox_9->setValue(tracksproperties->track_blur[8]);
    ui->spinBox_10->setValue(tracksproperties->track_blur[9]);
    ui->spinBox_11->setValue(tracksproperties->track_blur[10]);
    ui->spinBox_12->setValue(tracksproperties->track_blur[11]);
    ui->spinBox_13->setValue(tracksproperties->track_blur[12]);
    ui->spinBox_14->setValue(tracksproperties->track_blur[13]);
    ui->spinBox_15->setValue(tracksproperties->track_blur[14]);
    ui->spinBox_16->setValue(tracksproperties->track_blur[15]);
    ui->spinBox_17->setValue(tracksproperties->track_blur[16]);
    ui->spinBox_18->setValue(tracksproperties->track_blur[17]);
    ui->spinBox_19->setValue(tracksproperties->track_blur[18]);
    ui->spinBox_20->setValue(tracksproperties->track_blur[19]);
    ui->spinBox_21->setValue(tracksproperties->track_blur[20]);
    ui->spinBox_22->setValue(tracksproperties->track_blur[21]);
    ui->spinBox_23->setValue(tracksproperties->track_blur[22]);
    ui->spinBox_24->setValue(tracksproperties->track_blur[23]);

    ui->notesConnect1->setCurrentIndex(tracksproperties->interconnect[0]);
    ui->notesConnect1_2->setCurrentIndex(tracksproperties->interconnect[1]);
    ui->notesConnect1_3->setCurrentIndex(tracksproperties->interconnect[2]);
    ui->notesConnect1_4->setCurrentIndex(tracksproperties->interconnect[3]);
    ui->notesConnect1_5->setCurrentIndex(tracksproperties->interconnect[4]);
    ui->notesConnect1_6->setCurrentIndex(tracksproperties->interconnect[5]);
    ui->notesConnect1_7->setCurrentIndex(tracksproperties->interconnect[6]);
    ui->notesConnect1_8->setCurrentIndex(tracksproperties->interconnect[7]);
    ui->notesConnect1_9->setCurrentIndex(tracksproperties->interconnect[8]);
    ui->notesConnect1_10->setCurrentIndex(tracksproperties->interconnect[9]);
    ui->notesConnect1_11->setCurrentIndex(tracksproperties->interconnect[10]);
    ui->notesConnect1_12->setCurrentIndex(tracksproperties->interconnect[11]);
    ui->notesConnect1_13->setCurrentIndex(tracksproperties->interconnect[12]);
    ui->notesConnect1_14->setCurrentIndex(tracksproperties->interconnect[13]);
    ui->notesConnect1_15->setCurrentIndex(tracksproperties->interconnect[14]);
    ui->notesConnect1_16->setCurrentIndex(tracksproperties->interconnect[15]);
    ui->notesConnect1_17->setCurrentIndex(tracksproperties->interconnect[16]);
    ui->notesConnect1_18->setCurrentIndex(tracksproperties->interconnect[17]);
    ui->notesConnect1_19->setCurrentIndex(tracksproperties->interconnect[18]);
    ui->notesConnect1_20->setCurrentIndex(tracksproperties->interconnect[19]);
    ui->notesConnect1_21->setCurrentIndex(tracksproperties->interconnect[20]);
    ui->notesConnect1_22->setCurrentIndex(tracksproperties->interconnect[21]);
    ui->notesConnect1_23->setCurrentIndex(tracksproperties->interconnect[22]);
    ui->notesConnect1_24->setCurrentIndex(tracksproperties->interconnect[23]);

    ui->colorScheme1->setCurrentIndex(tracksproperties->colorScheme[0]);
    ui->colorScheme1_2->setCurrentIndex(tracksproperties->colorScheme[1]);
    ui->colorScheme1_3->setCurrentIndex(tracksproperties->colorScheme[2]);
    ui->colorScheme1_4->setCurrentIndex(tracksproperties->colorScheme[3]);
    ui->colorScheme1_5->setCurrentIndex(tracksproperties->colorScheme[4]);
    ui->colorScheme1_6->setCurrentIndex(tracksproperties->colorScheme[5]);
    ui->colorScheme1_7->setCurrentIndex(tracksproperties->colorScheme[6]);
    ui->colorScheme1_8->setCurrentIndex(tracksproperties->colorScheme[7]);
    ui->colorScheme1_9->setCurrentIndex(tracksproperties->colorScheme[8]);
    ui->colorScheme1_10->setCurrentIndex(tracksproperties->colorScheme[9]);
    ui->colorScheme1_11->setCurrentIndex(tracksproperties->colorScheme[10]);
    ui->colorScheme1_12->setCurrentIndex(tracksproperties->colorScheme[11]);
    ui->colorScheme1_13->setCurrentIndex(tracksproperties->colorScheme[12]);
    ui->colorScheme1_14->setCurrentIndex(tracksproperties->colorScheme[13]);
    ui->colorScheme1_15->setCurrentIndex(tracksproperties->colorScheme[14]);
    ui->colorScheme1_16->setCurrentIndex(tracksproperties->colorScheme[15]);
    ui->colorScheme1_17->setCurrentIndex(tracksproperties->colorScheme[16]);
    ui->colorScheme1_18->setCurrentIndex(tracksproperties->colorScheme[17]);
    ui->colorScheme1_19->setCurrentIndex(tracksproperties->colorScheme[18]);
    ui->colorScheme1_20->setCurrentIndex(tracksproperties->colorScheme[19]);
    ui->colorScheme1_21->setCurrentIndex(tracksproperties->colorScheme[20]);
    ui->colorScheme1_22->setCurrentIndex(tracksproperties->colorScheme[21]);
    ui->colorScheme1_23->setCurrentIndex(tracksproperties->colorScheme[22]);
    ui->colorScheme1_24->setCurrentIndex(tracksproperties->colorScheme[23]);


    ui->checkBox->setText(QString::fromStdString(track_names->at(0)));
    ui->checkBox_2->setText(QString::fromStdString(track_names->at(1)));
    ui->checkBox_3->setText(QString::fromStdString(track_names->at(2)));
    ui->checkBox_2->setText(QString::fromStdString(track_names->at(1)));
    ui->checkBox_3->setText(QString::fromStdString(track_names->at(2)));
    ui->checkBox_4->setText(QString::fromStdString(track_names->at(3)));
    ui->checkBox_5->setText(QString::fromStdString(track_names->at(4)));
    ui->checkBox_6->setText(QString::fromStdString(track_names->at(5)));
    ui->checkBox_7->setText(QString::fromStdString(track_names->at(6)));
    ui->checkBox_8->setText(QString::fromStdString(track_names->at(7)));
    ui->checkBox_9->setText(QString::fromStdString(track_names->at(8)));
    ui->checkBox_10->setText(QString::fromStdString(track_names->at(9)));
    ui->checkBox_11->setText(QString::fromStdString(track_names->at(10)));
    ui->checkBox_12->setText(QString::fromStdString(track_names->at(11)));
    ui->checkBox_13->setText(QString::fromStdString(track_names->at(12)));
    ui->checkBox_14->setText(QString::fromStdString(track_names->at(13)));
    ui->checkBox_15->setText(QString::fromStdString(track_names->at(14)));
    ui->checkBox_16->setText(QString::fromStdString(track_names->at(15)));
    ui->checkBox_17->setText(QString::fromStdString(track_names->at(16)));
    ui->checkBox_18->setText(QString::fromStdString(track_names->at(17)));
    ui->checkBox_19->setText(QString::fromStdString(track_names->at(18)));
    ui->checkBox_20->setText(QString::fromStdString(track_names->at(19)));
    ui->checkBox_21->setText(QString::fromStdString(track_names->at(20)));
    ui->checkBox_22->setText(QString::fromStdString(track_names->at(21)));
    ui->checkBox_23->setText(QString::fromStdString(track_names->at(22)));
    ui->checkBox_24->setText(QString::fromStdString(track_names->at(23)));
}

DockWidgetTracks::~DockWidgetTracks()
{
    delete ui;
}

void DockWidgetTracks::on_pushButton_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(0,0),
        tracksproperties->getCv(0,1),
        tracksproperties->getCv(0,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier for the user)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(0,0,tcolor.red());
    tracksproperties->setCv(0,1,tcolor.green());
    tracksproperties->setCv(0,2,tcolor.blue());
    ct1.setRgb(tracksproperties->getCv(0,0), tracksproperties->getCv(0,1), tracksproperties->getCv(0,2));
    QPalette pal = ui->widget1->palette();
    pal.setColor(QPalette::Window, ct1);
    ui->widget1->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_2_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(1,0),
        tracksproperties->getCv(1,1),
        tracksproperties->getCv(1,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(1,0,tcolor.red());
    tracksproperties->setCv(1,1,tcolor.green());
    tracksproperties->setCv(1,2,tcolor.blue());
    ct2.setRgb(tracksproperties->getCv(1,0), tracksproperties->getCv(1,1), tracksproperties->getCv(1,2));
    QPalette pal = ui->widget1_2->palette();
    pal.setColor(QPalette::Window, ct2);
    ui->widget1_2->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_3_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(2,0),
        tracksproperties->getCv(2,1),
        tracksproperties->getCv(2,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(2,0,tcolor.red());
    tracksproperties->setCv(2,1,tcolor.green());
    tracksproperties->setCv(2,2,tcolor.blue());
    ct3.setRgb(tracksproperties->getCv(2,0), tracksproperties->getCv(2,1), tracksproperties->getCv(2,2));
    QPalette pal = ui->widget1_3->palette();
    pal.setColor(QPalette::Window, ct3);
    ui->widget1_3->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_4_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(3,0),
        tracksproperties->getCv(3,1),
        tracksproperties->getCv(3,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(3,0,tcolor.red());
    tracksproperties->setCv(3,1,tcolor.green());
    tracksproperties->setCv(3,2,tcolor.blue());
    ct4.setRgb(tracksproperties->getCv(3,0), tracksproperties->getCv(3,1), tracksproperties->getCv(3,2));
    QPalette pal = ui->widget1_4->palette();
    pal.setColor(QPalette::Window, ct4);
    ui->widget1_4->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_5_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(4,0),
        tracksproperties->getCv(4,1),
        tracksproperties->getCv(4,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(4,0,tcolor.red());
    tracksproperties->setCv(4,1,tcolor.green());
    tracksproperties->setCv(4,2,tcolor.blue());
    ct5.setRgb(tracksproperties->getCv(4,0), tracksproperties->getCv(4,1), tracksproperties->getCv(4,2));
    QPalette pal = ui->widget1_5->palette();
    pal.setColor(QPalette::Window, ct5);
    ui->widget1_5->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_6_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(5,0),
        tracksproperties->getCv(5,1),
        tracksproperties->getCv(5,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(5,0,tcolor.red());
    tracksproperties->setCv(5,1,tcolor.green());
    tracksproperties->setCv(5,2,tcolor.blue());
    ct6.setRgb(tracksproperties->getCv(5,0), tracksproperties->getCv(5,1), tracksproperties->getCv(5,2));
    QPalette pal = ui->widget1_6->palette();
    pal.setColor(QPalette::Window, ct6);
    ui->widget1_6->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_7_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(6,0),
        tracksproperties->getCv(6,1),
        tracksproperties->getCv(6,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(6,0,tcolor.red());
    tracksproperties->setCv(6,1,tcolor.green());
    tracksproperties->setCv(6,2,tcolor.blue());
    ct7.setRgb(tracksproperties->getCv(6,0), tracksproperties->getCv(6,1), tracksproperties->getCv(6,2));
    QPalette pal = ui->widget1_7->palette();
    pal.setColor(QPalette::Window, ct7);
    ui->widget1_7->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_8_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(7,0),
        tracksproperties->getCv(7,1),
        tracksproperties->getCv(7,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(7,0,tcolor.red());
    tracksproperties->setCv(7,1,tcolor.green());
    tracksproperties->setCv(7,2,tcolor.blue());
    ct8.setRgb(tracksproperties->getCv(7,0), tracksproperties->getCv(7,1), tracksproperties->getCv(7,2));
    QPalette pal = ui->widget1_8->palette();
    pal.setColor(QPalette::Window, ct8);
    ui->widget1_8->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_9_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(8,0),
        tracksproperties->getCv(8,1),
        tracksproperties->getCv(8,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(8,0,tcolor.red());
    tracksproperties->setCv(8,1,tcolor.green());
    tracksproperties->setCv(8,2,tcolor.blue());
    ct9.setRgb(tracksproperties->getCv(8,0), tracksproperties->getCv(8,1), tracksproperties->getCv(8,2));
    QPalette pal = ui->widget1_9->palette();
    pal.setColor(QPalette::Window, ct9);
    ui->widget1_9->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_10_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(9,0),
        tracksproperties->getCv(9,1),
        tracksproperties->getCv(9,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(9,0,tcolor.red());
    tracksproperties->setCv(9,1,tcolor.green());
    tracksproperties->setCv(9,2,tcolor.blue());
    ct10.setRgb(tracksproperties->getCv(9,0), tracksproperties->getCv(9,1), tracksproperties->getCv(9,2));
    QPalette pal = ui->widget1_10->palette();
    pal.setColor(QPalette::Window, ct10);
    ui->widget1_10->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_11_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(10,0),
        tracksproperties->getCv(10,1),
        tracksproperties->getCv(10,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(10,0,tcolor.red());
    tracksproperties->setCv(10,1,tcolor.green());
    tracksproperties->setCv(10,2,tcolor.blue());
    ct11.setRgb(tracksproperties->getCv(10,0), tracksproperties->getCv(10,1), tracksproperties->getCv(10,2));
    QPalette pal = ui->widget1_11->palette();
    pal.setColor(QPalette::Window, ct11);
    ui->widget1_11->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_12_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(11,0),
        tracksproperties->getCv(11,1),
        tracksproperties->getCv(11,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(11,0,tcolor.red());
    tracksproperties->setCv(11,1,tcolor.green());
    tracksproperties->setCv(11,2,tcolor.blue());
    ct12.setRgb(tracksproperties->getCv(11,0), tracksproperties->getCv(11,1), tracksproperties->getCv(11,2));
    QPalette pal = ui->widget1_12->palette();
    pal.setColor(QPalette::Window, ct12);
    ui->widget1_12->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_13_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(12,0),
        tracksproperties->getCv(12,1),
        tracksproperties->getCv(12,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(12,0,tcolor.red());
    tracksproperties->setCv(12,1,tcolor.green());
    tracksproperties->setCv(12,2,tcolor.blue());
    ct13.setRgb(tracksproperties->getCv(12,0), tracksproperties->getCv(12,1), tracksproperties->getCv(12,2));
    QPalette pal = ui->widget1_13->palette();
    pal.setColor(QPalette::Window, ct13);
    ui->widget1_13->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_14_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(13,0),
        tracksproperties->getCv(13,1),
        tracksproperties->getCv(13,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(13,0,tcolor.red());
    tracksproperties->setCv(13,1,tcolor.green());
    tracksproperties->setCv(13,2,tcolor.blue());
    ct14.setRgb(tracksproperties->getCv(13,0), tracksproperties->getCv(13,1), tracksproperties->getCv(13,2));
    QPalette pal = ui->widget1_14->palette();
    pal.setColor(QPalette::Window, ct14);
    ui->widget1_14->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_15_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(14,0),
        tracksproperties->getCv(14,1),
        tracksproperties->getCv(14,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(14,0,tcolor.red());
    tracksproperties->setCv(14,1,tcolor.green());
    tracksproperties->setCv(14,2,tcolor.blue());
    ct15.setRgb(tracksproperties->getCv(14,0), tracksproperties->getCv(14,1), tracksproperties->getCv(14,2));
    QPalette pal = ui->widget1_15->palette();
    pal.setColor(QPalette::Window, ct15);
    ui->widget1_15->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_16_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(15,0),
        tracksproperties->getCv(15,1),
        tracksproperties->getCv(15,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(15,0,tcolor.red());
    tracksproperties->setCv(15,1,tcolor.green());
    tracksproperties->setCv(15,2,tcolor.blue());
    ct16.setRgb(tracksproperties->getCv(15,0), tracksproperties->getCv(15,1), tracksproperties->getCv(15,2));
    QPalette pal = ui->widget1_16->palette();
    pal.setColor(QPalette::Window, ct16);
    ui->widget1_16->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_17_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(16,0),
        tracksproperties->getCv(16,1),
        tracksproperties->getCv(16,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(16,0,tcolor.red());
    tracksproperties->setCv(16,1,tcolor.green());
    tracksproperties->setCv(16,2,tcolor.blue());
    ct17.setRgb(tracksproperties->getCv(16,0), tracksproperties->getCv(16,1), tracksproperties->getCv(16,2));
    QPalette pal = ui->widget1_17->palette();
    pal.setColor(QPalette::Window, ct17);
    ui->widget1_17->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_18_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(17,0),
        tracksproperties->getCv(17,1),
        tracksproperties->getCv(17,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(17,0,tcolor.red());
    tracksproperties->setCv(17,1,tcolor.green());
    tracksproperties->setCv(17,2,tcolor.blue());
    ct18.setRgb(tracksproperties->getCv(17,0), tracksproperties->getCv(17,1), tracksproperties->getCv(17,2));
    QPalette pal = ui->widget1_18->palette();
    pal.setColor(QPalette::Window, ct18);
    ui->widget1_18->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_19_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(18,0),
        tracksproperties->getCv(18,1),
        tracksproperties->getCv(18,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(18,0,tcolor.red());
    tracksproperties->setCv(18,1,tcolor.green());
    tracksproperties->setCv(18,2,tcolor.blue());
    ct19.setRgb(tracksproperties->getCv(18,0), tracksproperties->getCv(18,1), tracksproperties->getCv(18,2));
    QPalette pal = ui->widget1_19->palette();
    pal.setColor(QPalette::Window, ct19);
    ui->widget1_19->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_20_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(19,0),
        tracksproperties->getCv(19,1),
        tracksproperties->getCv(19,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(19,0,tcolor.red());
    tracksproperties->setCv(19,1,tcolor.green());
    tracksproperties->setCv(19,2,tcolor.blue());
    ct20.setRgb(tracksproperties->getCv(19,0), tracksproperties->getCv(19,1), tracksproperties->getCv(19,2));
    QPalette pal = ui->widget1_20->palette();
    pal.setColor(QPalette::Window, ct20);
    ui->widget1_20->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_21_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(20,0),
        tracksproperties->getCv(20,1),
        tracksproperties->getCv(20,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(20,0,tcolor.red());
    tracksproperties->setCv(20,1,tcolor.green());
    tracksproperties->setCv(20,2,tcolor.blue());
    ct21.setRgb(tracksproperties->getCv(20,0), tracksproperties->getCv(20,1), tracksproperties->getCv(20,2));
    QPalette pal = ui->widget1_21->palette();
    pal.setColor(QPalette::Window, ct21);
    ui->widget1_21->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_22_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(21,0),
        tracksproperties->getCv(21,1),
        tracksproperties->getCv(21,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(21,0,tcolor.red());
    tracksproperties->setCv(21,1,tcolor.green());
    tracksproperties->setCv(21,2,tcolor.blue());
    ct22.setRgb(tracksproperties->getCv(21,0), tracksproperties->getCv(21,1), tracksproperties->getCv(21,2));
    QPalette pal = ui->widget1_22->palette();
    pal.setColor(QPalette::Window, ct22);
    ui->widget1_22->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_23_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(22,0),
        tracksproperties->getCv(22,1),
        tracksproperties->getCv(22,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(22,0,tcolor.red());
    tracksproperties->setCv(22,1,tcolor.green());
    tracksproperties->setCv(22,2,tcolor.blue());
    ct23.setRgb(tracksproperties->getCv(22,0), tracksproperties->getCv(22,1), tracksproperties->getCv(22,2));
    QPalette pal = ui->widget1_23->palette();
    pal.setColor(QPalette::Window, ct23);
    ui->widget1_23->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_24_clicked()
{
    QColor tcolor;
    tcolor.setRgb(tracksproperties->getCv(23,0),
        tracksproperties->getCv(23,1),
        tracksproperties->getCv(23,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color

    tracksproperties->setCv(23,0,tcolor.red());
    tracksproperties->setCv(23,1,tcolor.green());
    tracksproperties->setCv(23,2,tcolor.blue());
    ct24.setRgb(tracksproperties->getCv(23,0), tracksproperties->getCv(23,1), tracksproperties->getCv(23,2));
    QPalette pal = ui->widget1_24->palette();
    pal.setColor(QPalette::Window, ct24);
    ui->widget1_24->setPalette(pal);
}



//void DockWidgetTracks::on_pushButton_2_clicked()
//{
//    QColor tcolor;
//    tcolor = QColorDialog::getColor(Qt::white, this);

//    tracksproperties->setCv(1,0,tcolor.red());
//    tracksproperties->setCv(1,1,tcolor.green());
//    tracksproperties->setCv(1,2,tcolor.blue());
//    ct2.setRgb(tracksproperties->getCv(1,0), tracksproperties->getCv(1,1), tracksproperties->getCv(1,2));
//    QPalette pal = ui->widget1_2->palette();
//    pal.setColor(QPalette::Window, ct2);
//    ui->widget1_2->setPalette(pal);
//}

void DockWidgetTracks::on_checkBox_toggled(bool checked)
{
    tracksproperties->active[0] = checked;
}

void DockWidgetTracks::on_checkBox_2_toggled(bool checked)
{
    tracksproperties->active[1] = checked;
}

void DockWidgetTracks::on_checkBox_3_toggled(bool checked)
{
    tracksproperties->active[2] = checked;
}

void DockWidgetTracks::on_checkBox_4_toggled(bool checked)
{
    tracksproperties->active[3] = checked;
}

void DockWidgetTracks::on_checkBox_5_toggled(bool checked)
{
    tracksproperties->active[4] = checked;
}

void DockWidgetTracks::on_checkBox_6_toggled(bool checked)
{
    tracksproperties->active[5] = checked;
}

void DockWidgetTracks::on_checkBox_7_toggled(bool checked)
{
    tracksproperties->active[6] = checked;
}

void DockWidgetTracks::on_checkBox_8_toggled(bool checked)
{
    tracksproperties->active[7] = checked;
}

void DockWidgetTracks::on_checkBox_9_toggled(bool checked)
{
    tracksproperties->active[8] = checked;
}

void DockWidgetTracks::on_checkBox_10_toggled(bool checked)
{
    tracksproperties->active[9] = checked;
}

void DockWidgetTracks::on_checkBox_11_toggled(bool checked)
{
    tracksproperties->active[10] = checked;
}

void DockWidgetTracks::on_checkBox_12_toggled(bool checked)
{
    tracksproperties->active[11] = checked;
}

void DockWidgetTracks::on_checkBox_13_toggled(bool checked)
{
    tracksproperties->active[12] = checked;
}
void DockWidgetTracks::on_checkBox_14_toggled(bool checked)
{
    tracksproperties->active[13] = checked;
}

void DockWidgetTracks::on_checkBox_15_toggled(bool checked)
{
    tracksproperties->active[14] = checked;
}

void DockWidgetTracks::on_checkBox_16_toggled(bool checked)
{
    tracksproperties->active[15] = checked;
}

void DockWidgetTracks::on_checkBox_17_toggled(bool checked)
{
    tracksproperties->active[16] = checked;
}

void DockWidgetTracks::on_checkBox_18_toggled(bool checked)
{
    tracksproperties->active[17] = checked;
}

void DockWidgetTracks::on_checkBox_19_toggled(bool checked)
{
    tracksproperties->active[18] = checked;
}

void DockWidgetTracks::on_checkBox_20_toggled(bool checked)
{
    tracksproperties->active[19] = checked;
}

void DockWidgetTracks::on_checkBox_21_toggled(bool checked)
{
    tracksproperties->active[20] = checked;
}

void DockWidgetTracks::on_checkBox_22_toggled(bool checked)
{
    tracksproperties->active[21] = checked;
}

void DockWidgetTracks::on_checkBox_23_toggled(bool checked)
{
    tracksproperties->active[22] = checked;
}

void DockWidgetTracks::on_checkBox_24_toggled(bool checked)
{
    tracksproperties->active[23] = checked;
}



void DockWidgetTracks::on_notesAppear1_currentIndexChanged(int index)
{
    tracksproperties->shape[0] = index;
}

void DockWidgetTracks::on_notesAppear1_2_currentIndexChanged(int index)
{
    tracksproperties->shape[1] = index;
}

void DockWidgetTracks::on_notesAppear1_3_currentIndexChanged(int index)
{
    tracksproperties->shape[2] = index;
}

void DockWidgetTracks::on_notesAppear1_4_currentIndexChanged(int index)
{
    tracksproperties->shape[3] = index;
}

void DockWidgetTracks::on_notesAppear1_5_currentIndexChanged(int index)
{
    tracksproperties->shape[4] = index;
}

void DockWidgetTracks::on_notesAppear1_6_currentIndexChanged(int index)
{
    tracksproperties->shape[5] = index;
}

void DockWidgetTracks::on_notesAppear1_7_currentIndexChanged(int index)
{
    tracksproperties->shape[6] = index;
}

void DockWidgetTracks::on_notesAppear1_8_currentIndexChanged(int index)
{
    tracksproperties->shape[7] = index;
}

void DockWidgetTracks::on_notesAppear1_9_currentIndexChanged(int index)
{
    tracksproperties->shape[8] = index;
}

void DockWidgetTracks::on_notesAppear1_10_currentIndexChanged(int index)
{
    tracksproperties->shape[9] = index;
}

void DockWidgetTracks::on_notesAppear1_11_currentIndexChanged(int index)
{
    tracksproperties->shape[10] = index;
}

void DockWidgetTracks::on_notesAppear1_12_currentIndexChanged(int index)
{
    tracksproperties->shape[11] = index;
}

void DockWidgetTracks::on_notesAppear1_13_currentIndexChanged(int index)
{
    tracksproperties->shape[12] = index;
}

void DockWidgetTracks::on_notesAppear1_14_currentIndexChanged(int index)
{
    tracksproperties->shape[13] = index;
}

void DockWidgetTracks::on_notesAppear1_15_currentIndexChanged(int index)
{
    tracksproperties->shape[14] = index;
}

void DockWidgetTracks::on_notesAppear1_16_currentIndexChanged(int index)
{
    tracksproperties->shape[15] = index;
}

void DockWidgetTracks::on_notesAppear1_17_currentIndexChanged(int index)
{
    tracksproperties->shape[16] = index;
}

void DockWidgetTracks::on_notesAppear1_18_currentIndexChanged(int index)
{
    tracksproperties->shape[17] = index;
}

void DockWidgetTracks::on_notesAppear1_19_currentIndexChanged(int index)
{
    tracksproperties->shape[18] = index;
}

void DockWidgetTracks::on_notesAppear1_20_currentIndexChanged(int index)
{
    tracksproperties->shape[19] = index;
}

void DockWidgetTracks::on_notesAppear1_21_currentIndexChanged(int index)
{
    tracksproperties->shape[20] = index;
}

void DockWidgetTracks::on_notesAppear1_22_currentIndexChanged(int index)
{
    tracksproperties->shape[21] = index;
}

void DockWidgetTracks::on_notesAppear1_23_currentIndexChanged(int index)
{
    tracksproperties->shape[22] = index;
}

void DockWidgetTracks::on_notesAppear1_24_currentIndexChanged(int index)
{
    tracksproperties->shape[23] = index;
}



void DockWidgetTracks::on_spinBox_valueChanged(int arg1)
{
    tracksproperties->track_blur[0] = arg1;
}

void DockWidgetTracks::on_spinBox_2_valueChanged(int arg1)
{
    tracksproperties->track_blur[1] = arg1;
}

void DockWidgetTracks::on_spinBox_3_valueChanged(int arg1)
{
    tracksproperties->track_blur[2] = arg1;
}

void DockWidgetTracks::on_spinBox_4_valueChanged(int arg1)
{
    tracksproperties->track_blur[3] = arg1;
}

void DockWidgetTracks::on_spinBox_5_valueChanged(int arg1)
{
    tracksproperties->track_blur[4] = arg1;
}

void DockWidgetTracks::on_spinBox_6_valueChanged(int arg1)
{
    tracksproperties->track_blur[5] = arg1;
}

void DockWidgetTracks::on_spinBox_7_valueChanged(int arg1)
{
    tracksproperties->track_blur[6] = arg1;
}

void DockWidgetTracks::on_spinBox_8_valueChanged(int arg1)
{
    tracksproperties->track_blur[7] = arg1;
}

void DockWidgetTracks::on_spinBox_9_valueChanged(int arg1)
{
    tracksproperties->track_blur[8] = arg1;
}

void DockWidgetTracks::on_spinBox_10_valueChanged(int arg1)
{
    tracksproperties->track_blur[9] = arg1;
}

void DockWidgetTracks::on_spinBox_11_valueChanged(int arg1)
{
    tracksproperties->track_blur[10] = arg1;
}

void DockWidgetTracks::on_spinBox_12_valueChanged(int arg1)
{
    tracksproperties->track_blur[11] = arg1;
}

void DockWidgetTracks::on_spinBox_13_valueChanged(int arg1)
{
    tracksproperties->track_blur[12] = arg1;
}

void DockWidgetTracks::on_spinBox_14_valueChanged(int arg1)
{
    tracksproperties->track_blur[13] = arg1;
}

void DockWidgetTracks::on_spinBox_15_valueChanged(int arg1)
{
    tracksproperties->track_blur[14] = arg1;
}

void DockWidgetTracks::on_spinBox_16_valueChanged(int arg1)
{
    tracksproperties->track_blur[15] = arg1;
}

void DockWidgetTracks::on_spinBox_17_valueChanged(int arg1)
{
    tracksproperties->track_blur[16] = arg1;
}

void DockWidgetTracks::on_spinBox_18_valueChanged(int arg1)
{
    tracksproperties->track_blur[17] = arg1;
}

void DockWidgetTracks::on_spinBox_19_valueChanged(int arg1)
{
    tracksproperties->track_blur[18] = arg1;
}

void DockWidgetTracks::on_spinBox_20_valueChanged(int arg1)
{
    tracksproperties->track_blur[19] = arg1;
}

void DockWidgetTracks::on_spinBox_21_valueChanged(int arg1)
{
    tracksproperties->track_blur[20] = arg1;
}

void DockWidgetTracks::on_spinBox_22_valueChanged(int arg1)
{
    tracksproperties->track_blur[21] = arg1;
}

void DockWidgetTracks::on_spinBox_23_valueChanged(int arg1)
{
    tracksproperties->track_blur[22] = arg1;
}

void DockWidgetTracks::on_spinBox_24_valueChanged(int arg1)
{
    tracksproperties->track_blur[23] = arg1;
}




void DockWidgetTracks::on_notesConnect1_currentIndexChanged(int index)
{
    tracksproperties->interconnect[0] = index;
}

void DockWidgetTracks::on_notesConnect1_2_currentIndexChanged(int index)
{
    tracksproperties->interconnect[1] = index;
}

void DockWidgetTracks::on_notesConnect1_3_currentIndexChanged(int index)
{
    tracksproperties->interconnect[2] = index;
}

void DockWidgetTracks::on_notesConnect1_4_currentIndexChanged(int index)
{
    tracksproperties->interconnect[3] = index;
}

void DockWidgetTracks::on_notesConnect1_5_currentIndexChanged(int index)
{
    tracksproperties->interconnect[4] = index;
}

void DockWidgetTracks::on_notesConnect1_6_currentIndexChanged(int index)
{
    tracksproperties->interconnect[5] = index;
}

void DockWidgetTracks::on_notesConnect1_7_currentIndexChanged(int index)
{
    tracksproperties->interconnect[6] = index;
}

void DockWidgetTracks::on_notesConnect1_8_currentIndexChanged(int index)
{
    tracksproperties->interconnect[7] = index;
}

void DockWidgetTracks::on_notesConnect1_9_currentIndexChanged(int index)
{
    tracksproperties->interconnect[8] = index;
}

void DockWidgetTracks::on_notesConnect1_10_currentIndexChanged(int index)
{
    tracksproperties->interconnect[9] = index;
}

void DockWidgetTracks::on_notesConnect1_11_currentIndexChanged(int index)
{
    tracksproperties->interconnect[10] = index;
}

void DockWidgetTracks::on_notesConnect1_12_currentIndexChanged(int index)
{
    tracksproperties->interconnect[11] = index;
}

void DockWidgetTracks::on_notesConnect1_13_currentIndexChanged(int index)
{
    tracksproperties->interconnect[12] = index;
}

void DockWidgetTracks::on_notesConnect1_14_currentIndexChanged(int index)
{
    tracksproperties->interconnect[13] = index;
}

void DockWidgetTracks::on_notesConnect1_15_currentIndexChanged(int index)
{
    tracksproperties->interconnect[14] = index;
}

void DockWidgetTracks::on_notesConnect1_16_currentIndexChanged(int index)
{
    tracksproperties->interconnect[15] = index;
}

void DockWidgetTracks::on_notesConnect1_17_currentIndexChanged(int index)
{
    tracksproperties->interconnect[16] = index;
}

void DockWidgetTracks::on_notesConnect1_18_currentIndexChanged(int index)
{
    tracksproperties->interconnect[17] = index;
}

void DockWidgetTracks::on_notesConnect1_19_currentIndexChanged(int index)
{
    tracksproperties->interconnect[18] = index;
}

void DockWidgetTracks::on_notesConnect1_20_currentIndexChanged(int index)
{
    tracksproperties->interconnect[19] = index;
}

void DockWidgetTracks::on_notesConnect1_21_currentIndexChanged(int index)
{
    tracksproperties->interconnect[20] = index;
}

void DockWidgetTracks::on_notesConnect1_22_currentIndexChanged(int index)
{
    tracksproperties->interconnect[21] = index;
}

void DockWidgetTracks::on_notesConnect1_23_currentIndexChanged(int index)
{
    tracksproperties->interconnect[22] = index;
}

void DockWidgetTracks::on_notesConnect1_24_currentIndexChanged(int index)
{
    tracksproperties->interconnect[23] = index;
}



void DockWidgetTracks::on_colorScheme1_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[0] = index;
}

void DockWidgetTracks::on_colorScheme1_2_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[1] = index;
}

void DockWidgetTracks::on_colorScheme1_3_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[2] = index;
}

void DockWidgetTracks::on_colorScheme1_4_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[3] = index;
}

void DockWidgetTracks::on_colorScheme1_5_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[4] = index;
}

void DockWidgetTracks::on_colorScheme1_6_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[5] = index;
}

void DockWidgetTracks::on_colorScheme1_7_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[6] = index;
}

void DockWidgetTracks::on_colorScheme1_8_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[7] = index;
}

void DockWidgetTracks::on_colorScheme1_9_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[8] = index;
}

void DockWidgetTracks::on_colorScheme1_10_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[9] = index;
}

void DockWidgetTracks::on_colorScheme1_11_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[10] = index;
}

void DockWidgetTracks::on_colorScheme1_12_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[11] = index;
}

void DockWidgetTracks::on_colorScheme1_13_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[12] = index;
}

void DockWidgetTracks::on_colorScheme1_14_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[13] = index;
}

void DockWidgetTracks::on_colorScheme1_15_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[14] = index;
}

void DockWidgetTracks::on_colorScheme1_16_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[15] = index;
}

void DockWidgetTracks::on_colorScheme1_17_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[16] = index;
}

void DockWidgetTracks::on_colorScheme1_18_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[16] = index;
}

void DockWidgetTracks::on_colorScheme1_19_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[18] = index;
}

void DockWidgetTracks::on_colorScheme1_20_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[19] = index;
}

void DockWidgetTracks::on_colorScheme1_21_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[20] = index;
}

void DockWidgetTracks::on_colorScheme1_22_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[21] = index;
}

void DockWidgetTracks::on_colorScheme1_23_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[22] = index;
}

void DockWidgetTracks::on_colorScheme1_24_currentIndexChanged(int index)
{
    tracksproperties->colorScheme[23] = index;
}
