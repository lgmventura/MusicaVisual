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

#ifndef RENDERSETUP_H
#define RENDERSETUP_H

#include <QDockWidget>

#include <QDialog>
#include <QColor>

#include "visual/animation.h"
#include "musicdata.h"
#include "visual/videorecorder.h"

namespace Ui {
class RenderWidget;
}

class RenderWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(RenderP *rProp, QWidget *parent = 0, MusicData *mdt = nullptr, VideoRecorder *vRec = nullptr);
    ~RenderWidget();

private slots:
    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_cmb_vLnManualDenom_currentIndexChanged(int index);

    void on_checkBox_3_toggled(bool checked);

    void on_checkBox_4_toggled(bool checked);

    void on_checkBox_5_toggled(bool checked);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_lineEdit_editingFinished();

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_checkBox_6_toggled(bool checked);

    void on_checkBox_7_toggled(bool checked);

    void on_checkBox_8_toggled(bool checked);

    void on_comboBox_currentIndexChanged(int index);

    void on_spinBox_8_valueChanged(int arg1);

    void on_spinBox_9_valueChanged(int arg1);

    void on_checkBox_9_toggled(bool checked);

    void on_pb_setClr_vlines_clicked();

    void on_pb_setClr_hlines_clicked();

    void on_cb_vLineTrack_toggled(bool checked);

    void on_sb_vLineTrack_valueChanged(int arg1);

    void on_cb_dispChordNames_toggled(bool checked);

    void on_cb_dispNoteNames_toggled(bool checked);

    void on_cb_dispChordStar_toggled(bool checked);

    void on_combox_chordStar_currentIndexChanged(int index);

    void on_cmb_dispNoteNamesWhere_currentIndexChanged(int index);

    void on_spb_chordStarOffset_valueChanged(int arg1);

    void on_cb_vLineTSig_toggled(bool checked);

    void on_cb_sharpFlat_currentIndexChanged(int index);

    void on_cb_AA_toggled(bool checked);

    void on_cmb_videoCodec_currentIndexChanged(int index);

    void on_spb_maxDistInterLines_valueChanged(int arg1);

private:
    Ui::RenderWidget *ui;
    RenderP *RProp;

    QColor hlines;
    QColor vlines;
    MusicData *Mdt;
    VideoRecorder *VRec;
};

class RenderWidMaps
{
private: RenderWidMaps() {}

public:
    static std::map<int, string> CMB_FOURCC_KV;
    static std::map<string, int> CMB_FOURCC_VK;
};

#endif // RENDERSETUP_H
