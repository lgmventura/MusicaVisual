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

#ifndef DOCKWIDGETTRACKS_H
#define DOCKWIDGETTRACKS_H

#include <QDockWidget>

#include "dialogcolorpicker.h"

namespace Ui {
class DockWidgetTracks;
}

class DockWidgetTracks : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidgetTracks(QWidget *parent = 0);
    ~DockWidgetTracks();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();



    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_checkBox_3_toggled(bool checked);

    void on_checkBox_4_toggled(bool checked);

    void on_checkBox_5_toggled(bool checked);

    void on_checkBox_6_toggled(bool checked);

    void on_checkBox_7_toggled(bool checked);

    void on_checkBox_8_toggled(bool checked);

    void on_checkBox_9_toggled(bool checked);

    void on_checkBox_10_toggled(bool checked);

    void on_checkBox_11_toggled(bool checked);

    void on_checkBox_12_toggled(bool checked);

    void on_checkBox_13_toggled(bool checked);

    void on_checkBox_14_toggled(bool checked);

    void on_checkBox_15_toggled(bool checked);

    void on_checkBox_16_toggled(bool checked);




    void on_notesAppear1_currentIndexChanged(int index);

    void on_notesAppear1_2_currentIndexChanged(int index);

    void on_notesAppear1_3_currentIndexChanged(int index);

    void on_notesAppear1_4_currentIndexChanged(int index);

    void on_notesAppear1_5_currentIndexChanged(int index);

    void on_notesAppear1_6_currentIndexChanged(int index);

    void on_notesAppear1_7_currentIndexChanged(int index);

    void on_notesAppear1_8_currentIndexChanged(int index);

    void on_notesAppear1_9_currentIndexChanged(int index);

    void on_notesAppear1_10_currentIndexChanged(int index);

    void on_notesAppear1_11_currentIndexChanged(int index);

    void on_notesAppear1_12_currentIndexChanged(int index);

    void on_notesAppear1_13_currentIndexChanged(int index);

    void on_notesAppear1_14_currentIndexChanged(int index);

    void on_notesAppear1_15_currentIndexChanged(int index);

    void on_notesAppear1_16_currentIndexChanged(int index);



    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

    void on_spinBox_9_valueChanged(int arg1);

    void on_spinBox_10_valueChanged(int arg1);

    void on_spinBox_11_valueChanged(int arg1);

    void on_spinBox_12_valueChanged(int arg1);

    void on_spinBox_13_valueChanged(int arg1);

    void on_spinBox_14_valueChanged(int arg1);

    void on_spinBox_15_valueChanged(int arg1);

    void on_spinBox_16_valueChanged(int arg1);



    void on_notesConnect1_currentIndexChanged(int index);

    void on_notesConnect1_2_currentIndexChanged(int index);

    void on_notesConnect1_3_currentIndexChanged(int index);

    void on_notesConnect1_4_currentIndexChanged(int index);

    void on_notesConnect1_5_currentIndexChanged(int index);

    void on_notesConnect1_6_currentIndexChanged(int index);

    void on_notesConnect1_7_currentIndexChanged(int index);

    void on_notesConnect1_8_currentIndexChanged(int index);

    void on_notesConnect1_9_currentIndexChanged(int index);

    void on_notesConnect1_10_currentIndexChanged(int index);

    void on_notesConnect1_11_currentIndexChanged(int index);

    void on_notesConnect1_12_currentIndexChanged(int index);

    void on_notesConnect1_13_currentIndexChanged(int index);

    void on_notesConnect1_14_currentIndexChanged(int index);

    void on_notesConnect1_15_currentIndexChanged(int index);

    void on_notesConnect1_16_currentIndexChanged(int index);



    void on_colorScheme1_currentIndexChanged(int index);

    void on_colorScheme1_2_currentIndexChanged(int index);

    void on_colorScheme1_3_currentIndexChanged(int index);

    void on_colorScheme1_4_currentIndexChanged(int index);

    void on_colorScheme1_5_currentIndexChanged(int index);

    void on_colorScheme1_6_currentIndexChanged(int index);

    void on_colorScheme1_7_currentIndexChanged(int index);

    void on_colorScheme1_8_currentIndexChanged(int index);

    void on_colorScheme1_9_currentIndexChanged(int index);

    void on_colorScheme1_10_currentIndexChanged(int index);

    void on_colorScheme1_11_currentIndexChanged(int index);

    void on_colorScheme1_12_currentIndexChanged(int index);

    void on_colorScheme1_13_currentIndexChanged(int index);

    void on_colorScheme1_14_currentIndexChanged(int index);

    void on_colorScheme1_15_currentIndexChanged(int index);

    void on_colorScheme1_16_currentIndexChanged(int index);

private:
    Ui::DockWidgetTracks *ui;
    DialogColorPicker *dcolorpicker;

    QColor ct1;
    QColor ct2;
    QColor ct3;
    QColor ct4;
    QColor ct5;
    QColor ct6;
    QColor ct7;
    QColor ct8;
    QColor ct9;
    QColor ct10;
    QColor ct11;
    QColor ct12;
    QColor ct13;
    QColor ct14;
    QColor ct15;
    QColor ct16;
};

#endif // DOCKWIDGETTRACKS_H
