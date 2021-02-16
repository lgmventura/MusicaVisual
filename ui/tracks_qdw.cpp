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

#include "ui/tracks_qdw.h"
#include "ui_tracks_qdw.h"

#include "mainwindow.h"

#include <QColor>
#include <QColorDialog>

//TracksP *TProp = new TracksP();
//extern TracksP *TProp;
//extern std::vector <std::string> *Mdt->TrackNames;

DockWidgetTracks::DockWidgetTracks(TracksP *tProp, QWidget *parent, MusicData *mdt) :
    QDockWidget(parent),
    ui(new Ui::DockWidgetTracks)
{
    ui->setupUi(this);
    this->TProp = tProp;
    this->Mdt = mdt;

    ct1.setRgb(TProp->getCv(0,0), TProp->getCv(0,1), TProp->getCv(0,2));
    QPalette pal = ui->widget1->palette();
    pal.setColor(QPalette::Window, ct1);
    ui->widget1->setPalette(pal);

    ct2.setRgb(TProp->getCv(1,0), TProp->getCv(1,1), TProp->getCv(1,2));
    QPalette pal2 = ui->widget1_2->palette();
    pal2.setColor(QPalette::Window, ct2);
    ui->widget1_2->setPalette(pal2);

    ct3.setRgb(TProp->getCv(2,0), TProp->getCv(2,1), TProp->getCv(2,2));
    QPalette pal3 = ui->widget1_3->palette();
    pal3.setColor(QPalette::Window, ct3);
    ui->widget1_3->setPalette(pal3);

    ct4.setRgb(TProp->getCv(3,0), TProp->getCv(3,1), TProp->getCv(3,2));
    QPalette pal4 = ui->widget1_4->palette();
    pal4.setColor(QPalette::Window, ct4);
    ui->widget1_4->setPalette(pal4);

    ct5.setRgb(TProp->getCv(4,0), TProp->getCv(4,1), TProp->getCv(4,2));
    QPalette pal5 = ui->widget1_5->palette();
    pal5.setColor(QPalette::Window, ct5);
    ui->widget1_5->setPalette(pal5);

    ct6.setRgb(TProp->getCv(5,0), TProp->getCv(5,1), TProp->getCv(5,2));
    QPalette pal6 = ui->widget1_6->palette();
    pal6.setColor(QPalette::Window, ct6);
    ui->widget1_6->setPalette(pal6);

    ct7.setRgb(TProp->getCv(6,0), TProp->getCv(6,1), TProp->getCv(6,2));
    QPalette pal7 = ui->widget1_7->palette();
    pal7.setColor(QPalette::Window, ct7);
    ui->widget1_7->setPalette(pal7);

    ct8.setRgb(TProp->getCv(7,0), TProp->getCv(7,1), TProp->getCv(7,2));
    QPalette pal8 = ui->widget1_8->palette();
    pal8.setColor(QPalette::Window, ct8);
    ui->widget1_8->setPalette(pal8);

    ct9.setRgb(TProp->getCv(8,0), TProp->getCv(8,1), TProp->getCv(8,2));
    QPalette pal9 = ui->widget1_9->palette();
    pal9.setColor(QPalette::Window, ct9);
    ui->widget1_9->setPalette(pal9);

    ct10.setRgb(TProp->getCv(9,0), TProp->getCv(9,1), TProp->getCv(9,2));
    QPalette pal10 = ui->widget1_10->palette();
    pal10.setColor(QPalette::Window, ct10);
    ui->widget1_10->setPalette(pal10);

    ct11.setRgb(TProp->getCv(10,0), TProp->getCv(10,1), TProp->getCv(10,2));
    QPalette pal11 = ui->widget1_11->palette();
    pal11.setColor(QPalette::Window, ct11);
    ui->widget1_11->setPalette(pal11);

    ct12.setRgb(TProp->getCv(11,0), TProp->getCv(11,1), TProp->getCv(11,2));
    QPalette pal12 = ui->widget1_12->palette();
    pal12.setColor(QPalette::Window, ct12);
    ui->widget1_12->setPalette(pal12);

    ct13.setRgb(TProp->getCv(12,0), TProp->getCv(12,1), TProp->getCv(12,2));
    QPalette pal13 = ui->widget1_13->palette();
    pal13.setColor(QPalette::Window, ct13);
    ui->widget1_13->setPalette(pal13);

    ct14.setRgb(TProp->getCv(13,0), TProp->getCv(13,1), TProp->getCv(13,2));
    QPalette pal14 = ui->widget1_14->palette();
    pal14.setColor(QPalette::Window, ct14);
    ui->widget1_14->setPalette(pal14);

    ct15.setRgb(TProp->getCv(14,0), TProp->getCv(14,1), TProp->getCv(14,2));
    QPalette pal15 = ui->widget1_15->palette();
    pal15.setColor(QPalette::Window, ct15);
    ui->widget1_15->setPalette(pal15);

    ct16.setRgb(TProp->getCv(15,0), TProp->getCv(15,1), TProp->getCv(15,2));
    QPalette pal16 = ui->widget1_16->palette();
    pal16.setColor(QPalette::Window, ct16);
    ui->widget1_16->setPalette(pal16);

    ct17.setRgb(TProp->getCv(16,0), TProp->getCv(16,1), TProp->getCv(16,2));
    QPalette pal17 = ui->widget1_17->palette();
    pal17.setColor(QPalette::Window, ct17);
    ui->widget1_17->setPalette(pal17);

    ct18.setRgb(TProp->getCv(17,0), TProp->getCv(17,1), TProp->getCv(17,2));
    QPalette pal18 = ui->widget1_18->palette();
    pal18.setColor(QPalette::Window, ct18);
    ui->widget1_18->setPalette(pal18);

    ct19.setRgb(TProp->getCv(18,0), TProp->getCv(18,1), TProp->getCv(18,2));
    QPalette pal19= ui->widget1_19->palette();
    pal19.setColor(QPalette::Window, ct19);
    ui->widget1_19->setPalette(pal19);

    ct20.setRgb(TProp->getCv(19,0), TProp->getCv(19,1), TProp->getCv(19,2));
    QPalette pal20= ui->widget1_20->palette();
    pal20.setColor(QPalette::Window, ct20);
    ui->widget1_20->setPalette(pal20);

    ct21.setRgb(TProp->getCv(20,0), TProp->getCv(20,1), TProp->getCv(20,2));
    QPalette pal21= ui->widget1_21->palette();
    pal21.setColor(QPalette::Window, ct21);
    ui->widget1_21->setPalette(pal21);

    ct22.setRgb(TProp->getCv(21,0), TProp->getCv(21,1), TProp->getCv(21,2));
    QPalette pal22= ui->widget1_22->palette();
    pal22.setColor(QPalette::Window, ct22);
    ui->widget1_22->setPalette(pal22);

    ct23.setRgb(TProp->getCv(22,0), TProp->getCv(22,1), TProp->getCv(22,2));
    QPalette pal23= ui->widget1_23->palette();
    pal23.setColor(QPalette::Window, ct23);
    ui->widget1_23->setPalette(pal23);

    ct24.setRgb(TProp->getCv(23,0), TProp->getCv(23,1), TProp->getCv(23,2));
    QPalette pal24= ui->widget1_24->palette();
    pal24.setColor(QPalette::Window, ct24);
    ui->widget1_24->setPalette(pal24);



    ui->notesAppear1->setCurrentIndex(TProp->shape[0]);
    ui->notesAppear1_2->setCurrentIndex(TProp->shape[1]);
    ui->notesAppear1_3->setCurrentIndex(TProp->shape[2]);
    ui->notesAppear1_4->setCurrentIndex(TProp->shape[3]);
    ui->notesAppear1_5->setCurrentIndex(TProp->shape[4]);
    ui->notesAppear1_6->setCurrentIndex(TProp->shape[5]);
    ui->notesAppear1_7->setCurrentIndex(TProp->shape[6]);
    ui->notesAppear1_8->setCurrentIndex(TProp->shape[7]);
    ui->notesAppear1_9->setCurrentIndex(TProp->shape[8]);
    ui->notesAppear1_10->setCurrentIndex(TProp->shape[9]);
    ui->notesAppear1_11->setCurrentIndex(TProp->shape[10]);
    ui->notesAppear1_12->setCurrentIndex(TProp->shape[11]);
    ui->notesAppear1_13->setCurrentIndex(TProp->shape[12]);
    ui->notesAppear1_14->setCurrentIndex(TProp->shape[13]);
    ui->notesAppear1_15->setCurrentIndex(TProp->shape[14]);
    ui->notesAppear1_16->setCurrentIndex(TProp->shape[15]);
    ui->notesAppear1_17->setCurrentIndex(TProp->shape[16]);
    ui->notesAppear1_18->setCurrentIndex(TProp->shape[17]);
    ui->notesAppear1_19->setCurrentIndex(TProp->shape[18]);
    ui->notesAppear1_20->setCurrentIndex(TProp->shape[19]);
    ui->notesAppear1_21->setCurrentIndex(TProp->shape[20]);
    ui->notesAppear1_22->setCurrentIndex(TProp->shape[21]);
    ui->notesAppear1_23->setCurrentIndex(TProp->shape[22]);
    ui->notesAppear1_24->setCurrentIndex(TProp->shape[23]);

    ui->spinBox->setValue(TProp->track_blur[0]);
    ui->spinBox_2->setValue(TProp->track_blur[1]);
    ui->spinBox_3->setValue(TProp->track_blur[2]);
    ui->spinBox_4->setValue(TProp->track_blur[3]);
    ui->spinBox_5->setValue(TProp->track_blur[4]);
    ui->spinBox_6->setValue(TProp->track_blur[5]);
    ui->spinBox_7->setValue(TProp->track_blur[6]);
    ui->spinBox_8->setValue(TProp->track_blur[7]);
    ui->spinBox_9->setValue(TProp->track_blur[8]);
    ui->spinBox_10->setValue(TProp->track_blur[9]);
    ui->spinBox_11->setValue(TProp->track_blur[10]);
    ui->spinBox_12->setValue(TProp->track_blur[11]);
    ui->spinBox_13->setValue(TProp->track_blur[12]);
    ui->spinBox_14->setValue(TProp->track_blur[13]);
    ui->spinBox_15->setValue(TProp->track_blur[14]);
    ui->spinBox_16->setValue(TProp->track_blur[15]);
    ui->spinBox_17->setValue(TProp->track_blur[16]);
    ui->spinBox_18->setValue(TProp->track_blur[17]);
    ui->spinBox_19->setValue(TProp->track_blur[18]);
    ui->spinBox_20->setValue(TProp->track_blur[19]);
    ui->spinBox_21->setValue(TProp->track_blur[20]);
    ui->spinBox_22->setValue(TProp->track_blur[21]);
    ui->spinBox_23->setValue(TProp->track_blur[22]);
    ui->spinBox_24->setValue(TProp->track_blur[23]);

    ui->notesConnect1->setCurrentIndex(TProp->interconnect[0]);
    ui->notesConnect1_2->setCurrentIndex(TProp->interconnect[1]);
    ui->notesConnect1_3->setCurrentIndex(TProp->interconnect[2]);
    ui->notesConnect1_4->setCurrentIndex(TProp->interconnect[3]);
    ui->notesConnect1_5->setCurrentIndex(TProp->interconnect[4]);
    ui->notesConnect1_6->setCurrentIndex(TProp->interconnect[5]);
    ui->notesConnect1_7->setCurrentIndex(TProp->interconnect[6]);
    ui->notesConnect1_8->setCurrentIndex(TProp->interconnect[7]);
    ui->notesConnect1_9->setCurrentIndex(TProp->interconnect[8]);
    ui->notesConnect1_10->setCurrentIndex(TProp->interconnect[9]);
    ui->notesConnect1_11->setCurrentIndex(TProp->interconnect[10]);
    ui->notesConnect1_12->setCurrentIndex(TProp->interconnect[11]);
    ui->notesConnect1_13->setCurrentIndex(TProp->interconnect[12]);
    ui->notesConnect1_14->setCurrentIndex(TProp->interconnect[13]);
    ui->notesConnect1_15->setCurrentIndex(TProp->interconnect[14]);
    ui->notesConnect1_16->setCurrentIndex(TProp->interconnect[15]);
    ui->notesConnect1_17->setCurrentIndex(TProp->interconnect[16]);
    ui->notesConnect1_18->setCurrentIndex(TProp->interconnect[17]);
    ui->notesConnect1_19->setCurrentIndex(TProp->interconnect[18]);
    ui->notesConnect1_20->setCurrentIndex(TProp->interconnect[19]);
    ui->notesConnect1_21->setCurrentIndex(TProp->interconnect[20]);
    ui->notesConnect1_22->setCurrentIndex(TProp->interconnect[21]);
    ui->notesConnect1_23->setCurrentIndex(TProp->interconnect[22]);
    ui->notesConnect1_24->setCurrentIndex(TProp->interconnect[23]);

    ui->colorScheme1->setCurrentIndex(TProp->colorScheme[0]);
    ui->colorScheme1_2->setCurrentIndex(TProp->colorScheme[1]);
    ui->colorScheme1_3->setCurrentIndex(TProp->colorScheme[2]);
    ui->colorScheme1_4->setCurrentIndex(TProp->colorScheme[3]);
    ui->colorScheme1_5->setCurrentIndex(TProp->colorScheme[4]);
    ui->colorScheme1_6->setCurrentIndex(TProp->colorScheme[5]);
    ui->colorScheme1_7->setCurrentIndex(TProp->colorScheme[6]);
    ui->colorScheme1_8->setCurrentIndex(TProp->colorScheme[7]);
    ui->colorScheme1_9->setCurrentIndex(TProp->colorScheme[8]);
    ui->colorScheme1_10->setCurrentIndex(TProp->colorScheme[9]);
    ui->colorScheme1_11->setCurrentIndex(TProp->colorScheme[10]);
    ui->colorScheme1_12->setCurrentIndex(TProp->colorScheme[11]);
    ui->colorScheme1_13->setCurrentIndex(TProp->colorScheme[12]);
    ui->colorScheme1_14->setCurrentIndex(TProp->colorScheme[13]);
    ui->colorScheme1_15->setCurrentIndex(TProp->colorScheme[14]);
    ui->colorScheme1_16->setCurrentIndex(TProp->colorScheme[15]);
    ui->colorScheme1_17->setCurrentIndex(TProp->colorScheme[16]);
    ui->colorScheme1_18->setCurrentIndex(TProp->colorScheme[17]);
    ui->colorScheme1_19->setCurrentIndex(TProp->colorScheme[18]);
    ui->colorScheme1_20->setCurrentIndex(TProp->colorScheme[19]);
    ui->colorScheme1_21->setCurrentIndex(TProp->colorScheme[20]);
    ui->colorScheme1_22->setCurrentIndex(TProp->colorScheme[21]);
    ui->colorScheme1_23->setCurrentIndex(TProp->colorScheme[22]);
    ui->colorScheme1_24->setCurrentIndex(TProp->colorScheme[23]);

    if (Mdt != nullptr) // will crash if trying to access and it's a null pointer
    {
        ui->checkBox->setText(QString::fromStdString(Mdt->TrackNames.at(0)));
        ui->checkBox_2->setText(QString::fromStdString(Mdt->TrackNames.at(1)));
        ui->checkBox_3->setText(QString::fromStdString(Mdt->TrackNames.at(2)));
        ui->checkBox_2->setText(QString::fromStdString(Mdt->TrackNames.at(1)));
        ui->checkBox_3->setText(QString::fromStdString(Mdt->TrackNames.at(2)));
        ui->checkBox_4->setText(QString::fromStdString(Mdt->TrackNames.at(3)));
        ui->checkBox_5->setText(QString::fromStdString(Mdt->TrackNames.at(4)));
        ui->checkBox_6->setText(QString::fromStdString(Mdt->TrackNames.at(5)));
        ui->checkBox_7->setText(QString::fromStdString(Mdt->TrackNames.at(6)));
        ui->checkBox_8->setText(QString::fromStdString(Mdt->TrackNames.at(7)));
        ui->checkBox_9->setText(QString::fromStdString(Mdt->TrackNames.at(8)));
        ui->checkBox_10->setText(QString::fromStdString(Mdt->TrackNames.at(9)));
        ui->checkBox_11->setText(QString::fromStdString(Mdt->TrackNames.at(10)));
        ui->checkBox_12->setText(QString::fromStdString(Mdt->TrackNames.at(11)));
        ui->checkBox_13->setText(QString::fromStdString(Mdt->TrackNames.at(12)));
        ui->checkBox_14->setText(QString::fromStdString(Mdt->TrackNames.at(13)));
        ui->checkBox_15->setText(QString::fromStdString(Mdt->TrackNames.at(14)));
        ui->checkBox_16->setText(QString::fromStdString(Mdt->TrackNames.at(15)));
        ui->checkBox_17->setText(QString::fromStdString(Mdt->TrackNames.at(16)));
        ui->checkBox_18->setText(QString::fromStdString(Mdt->TrackNames.at(17)));
        ui->checkBox_19->setText(QString::fromStdString(Mdt->TrackNames.at(18)));
        ui->checkBox_20->setText(QString::fromStdString(Mdt->TrackNames.at(19)));
        ui->checkBox_21->setText(QString::fromStdString(Mdt->TrackNames.at(20)));
        ui->checkBox_22->setText(QString::fromStdString(Mdt->TrackNames.at(21)));
        ui->checkBox_23->setText(QString::fromStdString(Mdt->TrackNames.at(22)));
        ui->checkBox_24->setText(QString::fromStdString(Mdt->TrackNames.at(23)));
    }
}

DockWidgetTracks::~DockWidgetTracks()
{
    delete ui;
}

void DockWidgetTracks::on_pushButton_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(0,0),
        TProp->getCv(0,1),
        TProp->getCv(0,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier for the user - upd20201208 - this is no longer valid for the new colour picker dialog, can be removed)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(0,0,tcolor.red());
    TProp->setCv(0,1,tcolor.green());
    TProp->setCv(0,2,tcolor.blue());
    ct1.setRgb(TProp->getCv(0,0), TProp->getCv(0,1), TProp->getCv(0,2));
    QPalette pal = ui->widget1->palette();
    pal.setColor(QPalette::Window, ct1);
    ui->widget1->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_2_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(1,0),
        TProp->getCv(1,1),
        TProp->getCv(1,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(1,0,tcolor.red());
    TProp->setCv(1,1,tcolor.green());
    TProp->setCv(1,2,tcolor.blue());
    ct2.setRgb(TProp->getCv(1,0), TProp->getCv(1,1), TProp->getCv(1,2));
    QPalette pal = ui->widget1_2->palette();
    pal.setColor(QPalette::Window, ct2);
    ui->widget1_2->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_3_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(2,0),
        TProp->getCv(2,1),
        TProp->getCv(2,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(2,0,tcolor.red());
    TProp->setCv(2,1,tcolor.green());
    TProp->setCv(2,2,tcolor.blue());
    ct3.setRgb(TProp->getCv(2,0), TProp->getCv(2,1), TProp->getCv(2,2));
    QPalette pal = ui->widget1_3->palette();
    pal.setColor(QPalette::Window, ct3);
    ui->widget1_3->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_4_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(3,0),
        TProp->getCv(3,1),
        TProp->getCv(3,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(3,0,tcolor.red());
    TProp->setCv(3,1,tcolor.green());
    TProp->setCv(3,2,tcolor.blue());
    ct4.setRgb(TProp->getCv(3,0), TProp->getCv(3,1), TProp->getCv(3,2));
    QPalette pal = ui->widget1_4->palette();
    pal.setColor(QPalette::Window, ct4);
    ui->widget1_4->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_5_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(4,0),
        TProp->getCv(4,1),
        TProp->getCv(4,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(4,0,tcolor.red());
    TProp->setCv(4,1,tcolor.green());
    TProp->setCv(4,2,tcolor.blue());
    ct5.setRgb(TProp->getCv(4,0), TProp->getCv(4,1), TProp->getCv(4,2));
    QPalette pal = ui->widget1_5->palette();
    pal.setColor(QPalette::Window, ct5);
    ui->widget1_5->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_6_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(5,0),
        TProp->getCv(5,1),
        TProp->getCv(5,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(5,0,tcolor.red());
    TProp->setCv(5,1,tcolor.green());
    TProp->setCv(5,2,tcolor.blue());
    ct6.setRgb(TProp->getCv(5,0), TProp->getCv(5,1), TProp->getCv(5,2));
    QPalette pal = ui->widget1_6->palette();
    pal.setColor(QPalette::Window, ct6);
    ui->widget1_6->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_7_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(6,0),
        TProp->getCv(6,1),
        TProp->getCv(6,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(6,0,tcolor.red());
    TProp->setCv(6,1,tcolor.green());
    TProp->setCv(6,2,tcolor.blue());
    ct7.setRgb(TProp->getCv(6,0), TProp->getCv(6,1), TProp->getCv(6,2));
    QPalette pal = ui->widget1_7->palette();
    pal.setColor(QPalette::Window, ct7);
    ui->widget1_7->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_8_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(7,0),
        TProp->getCv(7,1),
        TProp->getCv(7,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(7,0,tcolor.red());
    TProp->setCv(7,1,tcolor.green());
    TProp->setCv(7,2,tcolor.blue());
    ct8.setRgb(TProp->getCv(7,0), TProp->getCv(7,1), TProp->getCv(7,2));
    QPalette pal = ui->widget1_8->palette();
    pal.setColor(QPalette::Window, ct8);
    ui->widget1_8->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_9_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(8,0),
        TProp->getCv(8,1),
        TProp->getCv(8,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(8,0,tcolor.red());
    TProp->setCv(8,1,tcolor.green());
    TProp->setCv(8,2,tcolor.blue());
    ct9.setRgb(TProp->getCv(8,0), TProp->getCv(8,1), TProp->getCv(8,2));
    QPalette pal = ui->widget1_9->palette();
    pal.setColor(QPalette::Window, ct9);
    ui->widget1_9->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_10_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(9,0),
        TProp->getCv(9,1),
        TProp->getCv(9,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(9,0,tcolor.red());
    TProp->setCv(9,1,tcolor.green());
    TProp->setCv(9,2,tcolor.blue());
    ct10.setRgb(TProp->getCv(9,0), TProp->getCv(9,1), TProp->getCv(9,2));
    QPalette pal = ui->widget1_10->palette();
    pal.setColor(QPalette::Window, ct10);
    ui->widget1_10->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_11_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(10,0),
        TProp->getCv(10,1),
        TProp->getCv(10,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(10,0,tcolor.red());
    TProp->setCv(10,1,tcolor.green());
    TProp->setCv(10,2,tcolor.blue());
    ct11.setRgb(TProp->getCv(10,0), TProp->getCv(10,1), TProp->getCv(10,2));
    QPalette pal = ui->widget1_11->palette();
    pal.setColor(QPalette::Window, ct11);
    ui->widget1_11->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_12_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(11,0),
        TProp->getCv(11,1),
        TProp->getCv(11,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(11,0,tcolor.red());
    TProp->setCv(11,1,tcolor.green());
    TProp->setCv(11,2,tcolor.blue());
    ct12.setRgb(TProp->getCv(11,0), TProp->getCv(11,1), TProp->getCv(11,2));
    QPalette pal = ui->widget1_12->palette();
    pal.setColor(QPalette::Window, ct12);
    ui->widget1_12->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_13_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(12,0),
        TProp->getCv(12,1),
        TProp->getCv(12,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(12,0,tcolor.red());
    TProp->setCv(12,1,tcolor.green());
    TProp->setCv(12,2,tcolor.blue());
    ct13.setRgb(TProp->getCv(12,0), TProp->getCv(12,1), TProp->getCv(12,2));
    QPalette pal = ui->widget1_13->palette();
    pal.setColor(QPalette::Window, ct13);
    ui->widget1_13->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_14_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(13,0),
        TProp->getCv(13,1),
        TProp->getCv(13,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(13,0,tcolor.red());
    TProp->setCv(13,1,tcolor.green());
    TProp->setCv(13,2,tcolor.blue());
    ct14.setRgb(TProp->getCv(13,0), TProp->getCv(13,1), TProp->getCv(13,2));
    QPalette pal = ui->widget1_14->palette();
    pal.setColor(QPalette::Window, ct14);
    ui->widget1_14->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_15_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(14,0),
        TProp->getCv(14,1),
        TProp->getCv(14,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(14,0,tcolor.red());
    TProp->setCv(14,1,tcolor.green());
    TProp->setCv(14,2,tcolor.blue());
    ct15.setRgb(TProp->getCv(14,0), TProp->getCv(14,1), TProp->getCv(14,2));
    QPalette pal = ui->widget1_15->palette();
    pal.setColor(QPalette::Window, ct15);
    ui->widget1_15->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_16_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(15,0),
        TProp->getCv(15,1),
        TProp->getCv(15,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(15,0,tcolor.red());
    TProp->setCv(15,1,tcolor.green());
    TProp->setCv(15,2,tcolor.blue());
    ct16.setRgb(TProp->getCv(15,0), TProp->getCv(15,1), TProp->getCv(15,2));
    QPalette pal = ui->widget1_16->palette();
    pal.setColor(QPalette::Window, ct16);
    ui->widget1_16->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_17_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(16,0),
        TProp->getCv(16,1),
        TProp->getCv(16,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(16,0,tcolor.red());
    TProp->setCv(16,1,tcolor.green());
    TProp->setCv(16,2,tcolor.blue());
    ct17.setRgb(TProp->getCv(16,0), TProp->getCv(16,1), TProp->getCv(16,2));
    QPalette pal = ui->widget1_17->palette();
    pal.setColor(QPalette::Window, ct17);
    ui->widget1_17->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_18_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(17,0),
        TProp->getCv(17,1),
        TProp->getCv(17,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(17,0,tcolor.red());
    TProp->setCv(17,1,tcolor.green());
    TProp->setCv(17,2,tcolor.blue());
    ct18.setRgb(TProp->getCv(17,0), TProp->getCv(17,1), TProp->getCv(17,2));
    QPalette pal = ui->widget1_18->palette();
    pal.setColor(QPalette::Window, ct18);
    ui->widget1_18->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_19_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(18,0),
        TProp->getCv(18,1),
        TProp->getCv(18,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(18,0,tcolor.red());
    TProp->setCv(18,1,tcolor.green());
    TProp->setCv(18,2,tcolor.blue());
    ct19.setRgb(TProp->getCv(18,0), TProp->getCv(18,1), TProp->getCv(18,2));
    QPalette pal = ui->widget1_19->palette();
    pal.setColor(QPalette::Window, ct19);
    ui->widget1_19->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_20_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(19,0),
        TProp->getCv(19,1),
        TProp->getCv(19,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(19,0,tcolor.red());
    TProp->setCv(19,1,tcolor.green());
    TProp->setCv(19,2,tcolor.blue());
    ct20.setRgb(TProp->getCv(19,0), TProp->getCv(19,1), TProp->getCv(19,2));
    QPalette pal = ui->widget1_20->palette();
    pal.setColor(QPalette::Window, ct20);
    ui->widget1_20->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_21_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(20,0),
        TProp->getCv(20,1),
        TProp->getCv(20,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(20,0,tcolor.red());
    TProp->setCv(20,1,tcolor.green());
    TProp->setCv(20,2,tcolor.blue());
    ct21.setRgb(TProp->getCv(20,0), TProp->getCv(20,1), TProp->getCv(20,2));
    QPalette pal = ui->widget1_21->palette();
    pal.setColor(QPalette::Window, ct21);
    ui->widget1_21->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_22_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(21,0),
        TProp->getCv(21,1),
        TProp->getCv(21,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(21,0,tcolor.red());
    TProp->setCv(21,1,tcolor.green());
    TProp->setCv(21,2,tcolor.blue());
    ct22.setRgb(TProp->getCv(21,0), TProp->getCv(21,1), TProp->getCv(21,2));
    QPalette pal = ui->widget1_22->palette();
    pal.setColor(QPalette::Window, ct22);
    ui->widget1_22->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_23_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(22,0),
        TProp->getCv(22,1),
        TProp->getCv(22,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(22,0,tcolor.red());
    TProp->setCv(22,1,tcolor.green());
    TProp->setCv(22,2,tcolor.blue());
    ct23.setRgb(TProp->getCv(22,0), TProp->getCv(22,1), TProp->getCv(22,2));
    QPalette pal = ui->widget1_23->palette();
    pal.setColor(QPalette::Window, ct23);
    ui->widget1_23->setPalette(pal);
}

void DockWidgetTracks::on_pushButton_24_clicked()
{
    QColor tcolor;
    tcolor.setRgb(TProp->getCv(23,0),
        TProp->getCv(23,1),
        TProp->getCv(23,2));
    if (tcolor.red() == 0 && tcolor.green() == 0 && tcolor.blue() == 0) // If the current color is black,
        tcolor = QColorDialog::getColor(Qt::white, this); //s tart color choosing from white (easier)
    else
        tcolor = QColorDialog::getColor(tcolor, this); // Otherwise, start from the current color
    if (!tcolor.isValid()) return; // when the user cancels the dialog, it returns an invalid colour that will be interpreted as black

    TProp->setCv(23,0,tcolor.red());
    TProp->setCv(23,1,tcolor.green());
    TProp->setCv(23,2,tcolor.blue());
    ct24.setRgb(TProp->getCv(23,0), TProp->getCv(23,1), TProp->getCv(23,2));
    QPalette pal = ui->widget1_24->palette();
    pal.setColor(QPalette::Window, ct24);
    ui->widget1_24->setPalette(pal);
}



//void DockWidgetTracks::on_pushButton_2_clicked()
//{
//    QColor tcolor;
//    tcolor = QColorDialog::getColor(Qt::white, this);

//    TProp->setCv(1,0,tcolor.red());
//    TProp->setCv(1,1,tcolor.green());
//    TProp->setCv(1,2,tcolor.blue());
//    ct2.setRgb(TProp->getCv(1,0), TProp->getCv(1,1), TProp->getCv(1,2));
//    QPalette pal = ui->widget1_2->palette();
//    pal.setColor(QPalette::Window, ct2);
//    ui->widget1_2->setPalette(pal);
//}

void DockWidgetTracks::on_checkBox_toggled(bool checked)
{
    TProp->active[0] = checked;
}

void DockWidgetTracks::on_checkBox_2_toggled(bool checked)
{
    TProp->active[1] = checked;
}

void DockWidgetTracks::on_checkBox_3_toggled(bool checked)
{
    TProp->active[2] = checked;
}

void DockWidgetTracks::on_checkBox_4_toggled(bool checked)
{
    TProp->active[3] = checked;
}

void DockWidgetTracks::on_checkBox_5_toggled(bool checked)
{
    TProp->active[4] = checked;
}

void DockWidgetTracks::on_checkBox_6_toggled(bool checked)
{
    TProp->active[5] = checked;
}

void DockWidgetTracks::on_checkBox_7_toggled(bool checked)
{
    TProp->active[6] = checked;
}

void DockWidgetTracks::on_checkBox_8_toggled(bool checked)
{
    TProp->active[7] = checked;
}

void DockWidgetTracks::on_checkBox_9_toggled(bool checked)
{
    TProp->active[8] = checked;
}

void DockWidgetTracks::on_checkBox_10_toggled(bool checked)
{
    TProp->active[9] = checked;
}

void DockWidgetTracks::on_checkBox_11_toggled(bool checked)
{
    TProp->active[10] = checked;
}

void DockWidgetTracks::on_checkBox_12_toggled(bool checked)
{
    TProp->active[11] = checked;
}

void DockWidgetTracks::on_checkBox_13_toggled(bool checked)
{
    TProp->active[12] = checked;
}
void DockWidgetTracks::on_checkBox_14_toggled(bool checked)
{
    TProp->active[13] = checked;
}

void DockWidgetTracks::on_checkBox_15_toggled(bool checked)
{
    TProp->active[14] = checked;
}

void DockWidgetTracks::on_checkBox_16_toggled(bool checked)
{
    TProp->active[15] = checked;
}

void DockWidgetTracks::on_checkBox_17_toggled(bool checked)
{
    TProp->active[16] = checked;
}

void DockWidgetTracks::on_checkBox_18_toggled(bool checked)
{
    TProp->active[17] = checked;
}

void DockWidgetTracks::on_checkBox_19_toggled(bool checked)
{
    TProp->active[18] = checked;
}

void DockWidgetTracks::on_checkBox_20_toggled(bool checked)
{
    TProp->active[19] = checked;
}

void DockWidgetTracks::on_checkBox_21_toggled(bool checked)
{
    TProp->active[20] = checked;
}

void DockWidgetTracks::on_checkBox_22_toggled(bool checked)
{
    TProp->active[21] = checked;
}

void DockWidgetTracks::on_checkBox_23_toggled(bool checked)
{
    TProp->active[22] = checked;
}

void DockWidgetTracks::on_checkBox_24_toggled(bool checked)
{
    TProp->active[23] = checked;
}



void DockWidgetTracks::on_notesAppear1_currentIndexChanged(int index)
{
    TProp->shape[0] = index;
}

void DockWidgetTracks::on_notesAppear1_2_currentIndexChanged(int index)
{
    TProp->shape[1] = index;
}

void DockWidgetTracks::on_notesAppear1_3_currentIndexChanged(int index)
{
    TProp->shape[2] = index;
}

void DockWidgetTracks::on_notesAppear1_4_currentIndexChanged(int index)
{
    TProp->shape[3] = index;
}

void DockWidgetTracks::on_notesAppear1_5_currentIndexChanged(int index)
{
    TProp->shape[4] = index;
}

void DockWidgetTracks::on_notesAppear1_6_currentIndexChanged(int index)
{
    TProp->shape[5] = index;
}

void DockWidgetTracks::on_notesAppear1_7_currentIndexChanged(int index)
{
    TProp->shape[6] = index;
}

void DockWidgetTracks::on_notesAppear1_8_currentIndexChanged(int index)
{
    TProp->shape[7] = index;
}

void DockWidgetTracks::on_notesAppear1_9_currentIndexChanged(int index)
{
    TProp->shape[8] = index;
}

void DockWidgetTracks::on_notesAppear1_10_currentIndexChanged(int index)
{
    TProp->shape[9] = index;
}

void DockWidgetTracks::on_notesAppear1_11_currentIndexChanged(int index)
{
    TProp->shape[10] = index;
}

void DockWidgetTracks::on_notesAppear1_12_currentIndexChanged(int index)
{
    TProp->shape[11] = index;
}

void DockWidgetTracks::on_notesAppear1_13_currentIndexChanged(int index)
{
    TProp->shape[12] = index;
}

void DockWidgetTracks::on_notesAppear1_14_currentIndexChanged(int index)
{
    TProp->shape[13] = index;
}

void DockWidgetTracks::on_notesAppear1_15_currentIndexChanged(int index)
{
    TProp->shape[14] = index;
}

void DockWidgetTracks::on_notesAppear1_16_currentIndexChanged(int index)
{
    TProp->shape[15] = index;
}

void DockWidgetTracks::on_notesAppear1_17_currentIndexChanged(int index)
{
    TProp->shape[16] = index;
}

void DockWidgetTracks::on_notesAppear1_18_currentIndexChanged(int index)
{
    TProp->shape[17] = index;
}

void DockWidgetTracks::on_notesAppear1_19_currentIndexChanged(int index)
{
    TProp->shape[18] = index;
}

void DockWidgetTracks::on_notesAppear1_20_currentIndexChanged(int index)
{
    TProp->shape[19] = index;
}

void DockWidgetTracks::on_notesAppear1_21_currentIndexChanged(int index)
{
    TProp->shape[20] = index;
}

void DockWidgetTracks::on_notesAppear1_22_currentIndexChanged(int index)
{
    TProp->shape[21] = index;
}

void DockWidgetTracks::on_notesAppear1_23_currentIndexChanged(int index)
{
    TProp->shape[22] = index;
}

void DockWidgetTracks::on_notesAppear1_24_currentIndexChanged(int index)
{
    TProp->shape[23] = index;
}



void DockWidgetTracks::on_spinBox_valueChanged(int arg1)
{
    TProp->track_blur[0] = arg1;
}

void DockWidgetTracks::on_spinBox_2_valueChanged(int arg1)
{
    TProp->track_blur[1] = arg1;
}

void DockWidgetTracks::on_spinBox_3_valueChanged(int arg1)
{
    TProp->track_blur[2] = arg1;
}

void DockWidgetTracks::on_spinBox_4_valueChanged(int arg1)
{
    TProp->track_blur[3] = arg1;
}

void DockWidgetTracks::on_spinBox_5_valueChanged(int arg1)
{
    TProp->track_blur[4] = arg1;
}

void DockWidgetTracks::on_spinBox_6_valueChanged(int arg1)
{
    TProp->track_blur[5] = arg1;
}

void DockWidgetTracks::on_spinBox_7_valueChanged(int arg1)
{
    TProp->track_blur[6] = arg1;
}

void DockWidgetTracks::on_spinBox_8_valueChanged(int arg1)
{
    TProp->track_blur[7] = arg1;
}

void DockWidgetTracks::on_spinBox_9_valueChanged(int arg1)
{
    TProp->track_blur[8] = arg1;
}

void DockWidgetTracks::on_spinBox_10_valueChanged(int arg1)
{
    TProp->track_blur[9] = arg1;
}

void DockWidgetTracks::on_spinBox_11_valueChanged(int arg1)
{
    TProp->track_blur[10] = arg1;
}

void DockWidgetTracks::on_spinBox_12_valueChanged(int arg1)
{
    TProp->track_blur[11] = arg1;
}

void DockWidgetTracks::on_spinBox_13_valueChanged(int arg1)
{
    TProp->track_blur[12] = arg1;
}

void DockWidgetTracks::on_spinBox_14_valueChanged(int arg1)
{
    TProp->track_blur[13] = arg1;
}

void DockWidgetTracks::on_spinBox_15_valueChanged(int arg1)
{
    TProp->track_blur[14] = arg1;
}

void DockWidgetTracks::on_spinBox_16_valueChanged(int arg1)
{
    TProp->track_blur[15] = arg1;
}

void DockWidgetTracks::on_spinBox_17_valueChanged(int arg1)
{
    TProp->track_blur[16] = arg1;
}

void DockWidgetTracks::on_spinBox_18_valueChanged(int arg1)
{
    TProp->track_blur[17] = arg1;
}

void DockWidgetTracks::on_spinBox_19_valueChanged(int arg1)
{
    TProp->track_blur[18] = arg1;
}

void DockWidgetTracks::on_spinBox_20_valueChanged(int arg1)
{
    TProp->track_blur[19] = arg1;
}

void DockWidgetTracks::on_spinBox_21_valueChanged(int arg1)
{
    TProp->track_blur[20] = arg1;
}

void DockWidgetTracks::on_spinBox_22_valueChanged(int arg1)
{
    TProp->track_blur[21] = arg1;
}

void DockWidgetTracks::on_spinBox_23_valueChanged(int arg1)
{
    TProp->track_blur[22] = arg1;
}

void DockWidgetTracks::on_spinBox_24_valueChanged(int arg1)
{
    TProp->track_blur[23] = arg1;
}




void DockWidgetTracks::on_notesConnect1_currentIndexChanged(int index)
{
    TProp->interconnect[0] = index;
}

void DockWidgetTracks::on_notesConnect1_2_currentIndexChanged(int index)
{
    TProp->interconnect[1] = index;
}

void DockWidgetTracks::on_notesConnect1_3_currentIndexChanged(int index)
{
    TProp->interconnect[2] = index;
}

void DockWidgetTracks::on_notesConnect1_4_currentIndexChanged(int index)
{
    TProp->interconnect[3] = index;
}

void DockWidgetTracks::on_notesConnect1_5_currentIndexChanged(int index)
{
    TProp->interconnect[4] = index;
}

void DockWidgetTracks::on_notesConnect1_6_currentIndexChanged(int index)
{
    TProp->interconnect[5] = index;
}

void DockWidgetTracks::on_notesConnect1_7_currentIndexChanged(int index)
{
    TProp->interconnect[6] = index;
}

void DockWidgetTracks::on_notesConnect1_8_currentIndexChanged(int index)
{
    TProp->interconnect[7] = index;
}

void DockWidgetTracks::on_notesConnect1_9_currentIndexChanged(int index)
{
    TProp->interconnect[8] = index;
}

void DockWidgetTracks::on_notesConnect1_10_currentIndexChanged(int index)
{
    TProp->interconnect[9] = index;
}

void DockWidgetTracks::on_notesConnect1_11_currentIndexChanged(int index)
{
    TProp->interconnect[10] = index;
}

void DockWidgetTracks::on_notesConnect1_12_currentIndexChanged(int index)
{
    TProp->interconnect[11] = index;
}

void DockWidgetTracks::on_notesConnect1_13_currentIndexChanged(int index)
{
    TProp->interconnect[12] = index;
}

void DockWidgetTracks::on_notesConnect1_14_currentIndexChanged(int index)
{
    TProp->interconnect[13] = index;
}

void DockWidgetTracks::on_notesConnect1_15_currentIndexChanged(int index)
{
    TProp->interconnect[14] = index;
}

void DockWidgetTracks::on_notesConnect1_16_currentIndexChanged(int index)
{
    TProp->interconnect[15] = index;
}

void DockWidgetTracks::on_notesConnect1_17_currentIndexChanged(int index)
{
    TProp->interconnect[16] = index;
}

void DockWidgetTracks::on_notesConnect1_18_currentIndexChanged(int index)
{
    TProp->interconnect[17] = index;
}

void DockWidgetTracks::on_notesConnect1_19_currentIndexChanged(int index)
{
    TProp->interconnect[18] = index;
}

void DockWidgetTracks::on_notesConnect1_20_currentIndexChanged(int index)
{
    TProp->interconnect[19] = index;
}

void DockWidgetTracks::on_notesConnect1_21_currentIndexChanged(int index)
{
    TProp->interconnect[20] = index;
}

void DockWidgetTracks::on_notesConnect1_22_currentIndexChanged(int index)
{
    TProp->interconnect[21] = index;
}

void DockWidgetTracks::on_notesConnect1_23_currentIndexChanged(int index)
{
    TProp->interconnect[22] = index;
}

void DockWidgetTracks::on_notesConnect1_24_currentIndexChanged(int index)
{
    TProp->interconnect[23] = index;
}



void DockWidgetTracks::on_colorScheme1_currentIndexChanged(int index)
{
    TProp->colorScheme[0] = index;
}

void DockWidgetTracks::on_colorScheme1_2_currentIndexChanged(int index)
{
    TProp->colorScheme[1] = index;
}

void DockWidgetTracks::on_colorScheme1_3_currentIndexChanged(int index)
{
    TProp->colorScheme[2] = index;
}

void DockWidgetTracks::on_colorScheme1_4_currentIndexChanged(int index)
{
    TProp->colorScheme[3] = index;
}

void DockWidgetTracks::on_colorScheme1_5_currentIndexChanged(int index)
{
    TProp->colorScheme[4] = index;
}

void DockWidgetTracks::on_colorScheme1_6_currentIndexChanged(int index)
{
    TProp->colorScheme[5] = index;
}

void DockWidgetTracks::on_colorScheme1_7_currentIndexChanged(int index)
{
    TProp->colorScheme[6] = index;
}

void DockWidgetTracks::on_colorScheme1_8_currentIndexChanged(int index)
{
    TProp->colorScheme[7] = index;
}

void DockWidgetTracks::on_colorScheme1_9_currentIndexChanged(int index)
{
    TProp->colorScheme[8] = index;
}

void DockWidgetTracks::on_colorScheme1_10_currentIndexChanged(int index)
{
    TProp->colorScheme[9] = index;
}

void DockWidgetTracks::on_colorScheme1_11_currentIndexChanged(int index)
{
    TProp->colorScheme[10] = index;
}

void DockWidgetTracks::on_colorScheme1_12_currentIndexChanged(int index)
{
    TProp->colorScheme[11] = index;
}

void DockWidgetTracks::on_colorScheme1_13_currentIndexChanged(int index)
{
    TProp->colorScheme[12] = index;
}

void DockWidgetTracks::on_colorScheme1_14_currentIndexChanged(int index)
{
    TProp->colorScheme[13] = index;
}

void DockWidgetTracks::on_colorScheme1_15_currentIndexChanged(int index)
{
    TProp->colorScheme[14] = index;
}

void DockWidgetTracks::on_colorScheme1_16_currentIndexChanged(int index)
{
    TProp->colorScheme[15] = index;
}

void DockWidgetTracks::on_colorScheme1_17_currentIndexChanged(int index)
{
    TProp->colorScheme[16] = index;
}

void DockWidgetTracks::on_colorScheme1_18_currentIndexChanged(int index)
{
    TProp->colorScheme[16] = index;
}

void DockWidgetTracks::on_colorScheme1_19_currentIndexChanged(int index)
{
    TProp->colorScheme[18] = index;
}

void DockWidgetTracks::on_colorScheme1_20_currentIndexChanged(int index)
{
    TProp->colorScheme[19] = index;
}

void DockWidgetTracks::on_colorScheme1_21_currentIndexChanged(int index)
{
    TProp->colorScheme[20] = index;
}

void DockWidgetTracks::on_colorScheme1_22_currentIndexChanged(int index)
{
    TProp->colorScheme[21] = index;
}

void DockWidgetTracks::on_colorScheme1_23_currentIndexChanged(int index)
{
    TProp->colorScheme[22] = index;
}

void DockWidgetTracks::on_colorScheme1_24_currentIndexChanged(int index)
{
    TProp->colorScheme[23] = index;
}
