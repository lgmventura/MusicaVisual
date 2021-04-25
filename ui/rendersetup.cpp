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

#include "ui/rendersetup.h"
#include "ui_rendersetup.h"
#include "mainwindow.h"
#include "chords.h"

#include <QColor>
#include <QColorDialog>

//extern RenderP *RProp;
//extern std::string *codec_fourcc;
//extern chords mdt->GChords;

RenderWidget::RenderWidget(RenderP *rProp, QWidget *parent, MusicData *mdt, VideoRecorder *vRec) :
    QDockWidget(parent),
    ui(new Ui::RenderWidget)
{
    ui->setupUi(this);
    // Set member variables:
    this->RProp = rProp;
    this->Mdt = mdt;
    this->VRec = vRec;

    // Set UI state to current state:
    ui->lineEdit->setEnabled(false);
    if (vRec != nullptr)
        ui->lineEdit->setText(QString::fromStdString(vRec->CodecFourCC));
    else
    {
        ui->cmb_videoCodec->setEnabled(false); // no valid video object (will crash if accessed)
    }
    // Tab 0: vertical range/shift
    ui->spinBox_3->setValue(RProp->vertRange);
    ui->spinBox_4->setValue(RProp->vertShift);

    // Tab 1: lines:
    ui->spinBox->setValue(RProp->beat_measure_manual[0]);
    ui->cmb_vLnManualDenom->setCurrentIndex(RProp->beat_measure_manual[1]);
    ui->checkBox_8->setChecked(RProp->hlines);
    ui->comboBox->setCurrentIndex(RProp->hlines_type);
    ui->spinBox_8->setValue(RProp->hlines_n);
    ui->spinBox_9->setMaximum(RProp->hlines_n - 1);
    ui->spinBox_9->setValue(RProp->hlines_offset);
    ui->checkBox_9->setChecked(RProp->half_shift);
    ui->checkBox->setChecked(RProp->lines[0]);
    ui->cb_vLineTSig->setChecked(RProp->lines[1]);
    ui->checkBox_2->setChecked(RProp->lines[2]);
    ui->cb_vLineTrack->setChecked(RProp->lines[3]);
    ui->sb_vLineTrack->setValue(RProp->vlines_track_n);

    hlines.setRgb(RProp->hlines_colour[0], RProp->hlines_colour[1], RProp->hlines_colour[2]);
    vlines.setRgb(RProp->vlines_colour[0], RProp->vlines_colour[1], RProp->vlines_colour[2]);
    QPalette pal_h = ui->widget_hlcolour->palette();
    QPalette pal_v = ui->widget_vlcolour->palette();
    pal_h.setColor(QPalette::Window, hlines);
    pal_v.setColor(QPalette::Window, vlines);
    ui->widget_hlcolour->setPalette(pal_h);
    ui->widget_vlcolour->setPalette(pal_v);

    // Tab 2: render settings:
    ui->spinBox_2->setValue(RProp->blur_size[0]);
    ui->spinBox_5->setValue(RProp->blur_size[1]);
    ui->spinBox_6->setValue(RProp->blur_size_movnotes[0]);
    ui->spinBox_7->setValue(RProp->blur_size_movnotes[1]);

    ui->checkBox_3->setChecked(RProp->sep_render[0]);
    ui->checkBox_4->setChecked(RProp->sep_render[1]);
    ui->checkBox_5->setChecked(RProp->sep_render[2]);
    if      (RProp->shapeLineType == cv::LINE_8)
        ui->cb_AA->setChecked(false);
    else if (RProp->shapeLineType == cv::LINE_AA)
        ui->cb_AA->setChecked(true);

    // Tab 3: chord analysis:
    ui->cb_sharpFlat->setCurrentIndex(RProp->accidentalSharp);
    ui->cb_dispChordStar->setChecked(RProp->chord_star);
    ui->cb_dispNoteNames->setChecked(RProp->note_names);
    ui->cb_dispChordNames->setChecked(RProp->chord_names);
    ui->combox_chordStar->setCurrentIndex(RProp->chord_star_type);
    ui->cmb_dispNoteNamesWhere->setCurrentIndex(RProp->note_names_where);
    ui->spb_chordStarOffset->setValue(RProp->turn_chord_circle);

    // Tab 4: extra time:
    ui->checkBox_6->setChecked(RProp->extra_time[0]);
    ui->checkBox_7->setChecked(RProp->extra_time[1]);

    // Tab 5: render video codec::
    ui->cmb_videoCodec->setCurrentIndex(RenderWidMaps::CMB_FOURCC_VK[VRec->CodecFourCC]);


}

RenderWidget::~RenderWidget()
{
    delete ui;
}

void RenderWidget::on_checkBox_toggled(bool checked)
{
    RProp->lines[0] = checked;
}

void RenderWidget::on_cb_vLineTSig_toggled(bool checked)
{
    RProp->lines[1] = checked;
}

void RenderWidget::on_checkBox_2_toggled(bool checked)
{
    RProp->lines[2] = checked;
}

void RenderWidget::on_cb_vLineTrack_toggled(bool checked)
{
    RProp->lines[3] = checked;
}

void RenderWidget::on_spinBox_valueChanged(int arg1)
{
    RProp->beat_measure_manual[0] = arg1;
}

void RenderWidget::on_cmb_vLnManualDenom_currentIndexChanged(int index)
{
    RProp->beat_measure_manual[1] = index;
}

void RenderWidget::on_checkBox_3_toggled(bool checked)
{
    RProp->sep_render[0] = checked;
}

void RenderWidget::on_checkBox_4_toggled(bool checked)
{
    RProp->sep_render[1] = checked;
}

void RenderWidget::on_checkBox_5_toggled(bool checked)
{
    RProp->sep_render[2] = checked;
}

void RenderWidget::on_spinBox_2_valueChanged(int arg1)
{
    RProp->blur_size[0] = arg1;
}

void RenderWidget::on_spinBox_5_valueChanged(int arg1)
{
    RProp->blur_size[1] = arg1;
}

void RenderWidget::on_spinBox_6_valueChanged(int arg1)
{
    RProp->blur_size_movnotes[0] = arg1;
}

void RenderWidget::on_spinBox_7_valueChanged(int arg1)
{
    RProp->blur_size_movnotes[1] = arg1;
}

void RenderWidget::on_lineEdit_editingFinished()
{
    VRec->CodecFourCC = ui->lineEdit->text().toStdString();
}

void RenderWidget::on_spinBox_3_valueChanged(int arg1)
{
    RProp->vertRange = arg1;
}

void RenderWidget::on_spinBox_4_valueChanged(int arg1)
{
    RProp->vertShift = arg1;
}

void RenderWidget::on_checkBox_6_toggled(bool checked)
{
    RProp->extra_time[0] = checked;
}

void RenderWidget::on_checkBox_7_toggled(bool checked)
{
    RProp->extra_time[1] = checked;
}

void RenderWidget::on_checkBox_8_toggled(bool checked)
{
    RProp->hlines = checked;
}

void RenderWidget::on_comboBox_currentIndexChanged(int index)
{
    RProp->hlines_type = index;
}

void RenderWidget::on_spinBox_8_valueChanged(int arg1)
{
    RProp->hlines_n = arg1;
    ui->spinBox_9->setMaximum(arg1 - 1);
}

void RenderWidget::on_spinBox_9_valueChanged(int arg1)
{
    RProp->hlines_offset = arg1;
}

void RenderWidget::on_checkBox_9_toggled(bool checked)
{
    RProp->half_shift = checked;
}

void RenderWidget::on_pb_setClr_vlines_clicked()
{
    QColor tcolor;
    tcolor.setRgb(RProp->vlines_colour[0],
                  RProp->vlines_colour[1],
                  RProp->vlines_colour[2]);
    tcolor = QColorDialog::getColor(tcolor, this, "Pick a colour for the vertical rythmic lines", QColorDialog::DontUseNativeDialog);
    if (!tcolor.isValid()) return;

    RProp->vlines_colour[0] = tcolor.red();
    RProp->vlines_colour[1] = tcolor.green();
    RProp->vlines_colour[2] = tcolor.blue();
    vlines.setRgb(RProp->vlines_colour[0], RProp->vlines_colour[1], RProp->vlines_colour[2]);
    QPalette pal_v = ui->widget_vlcolour->palette();
    pal_v.setColor(QPalette::Window, vlines);
    ui->widget_vlcolour->setPalette(pal_v);
}

void RenderWidget::on_pb_setClr_hlines_clicked()
{
    QColor tcolor;
    tcolor.setRgb(RProp->hlines_colour[0],
                  RProp->hlines_colour[1],
                  RProp->hlines_colour[2]);
    tcolor = QColorDialog::getColor(tcolor, this, "Pick a colour for the horizontal lines", QColorDialog::DontUseNativeDialog);
    if (!tcolor.isValid()) return;

    RProp->hlines_colour[0] = tcolor.red();
    RProp->hlines_colour[1] = tcolor.green();
    RProp->hlines_colour[2] = tcolor.blue();
    hlines.setRgb(RProp->hlines_colour[0], RProp->hlines_colour[1], RProp->hlines_colour[2]);
    QPalette pal_h = ui->widget_hlcolour->palette();
    pal_h.setColor(QPalette::Window, hlines);
    ui->widget_hlcolour->setPalette(pal_h);
}

void RenderWidget::on_sb_vLineTrack_valueChanged(int arg1)
{
    RProp->vlines_track_n = arg1;
}

void RenderWidget::on_cb_dispChordNames_toggled(bool checked)
{
    RProp->chord_names = checked;
}

void RenderWidget::on_cb_dispNoteNames_toggled(bool checked)
{
    RProp->note_names = checked;
}

void RenderWidget::on_cb_dispChordStar_toggled(bool checked)
{
    RProp->chord_star = checked;
}

void RenderWidget::on_combox_chordStar_currentIndexChanged(int index)
{
    if (index == 0)
    {
        RProp->chord_star_type = chord::circleOfSemitones;
    }
    else if (index == 1)
    {
        RProp->chord_star_type = chord::circleOfFifths;
    }
}

void RenderWidget::on_cmb_dispNoteNamesWhere_currentIndexChanged(int index)
{
    RProp->note_names_where = index;
}

void RenderWidget::on_spb_chordStarOffset_valueChanged(int arg1)
{
    RProp->turn_chord_circle = arg1;
}

void RenderWidget::on_cb_sharpFlat_currentIndexChanged(int index)
{
    if (index == 0)
        RProp->accidentalSharp = false;
    else if (index == 1)
        RProp->accidentalSharp = true;
}

void RenderWidget::on_cb_AA_toggled(bool checked)
{
    if (checked == true)
    {
        RProp->shapeLineType = cv::LINE_AA;
    }
    else
    {
        RProp->shapeLineType = cv::LINE_8;
    }
}

void RenderWidget::on_cmb_videoCodec_currentIndexChanged(int index)
{
    VRec->CodecFourCC = RenderWidMaps::CMB_FOURCC_KV[index];
    ui->lineEdit->setText(QString::fromStdString(RenderWidMaps::CMB_FOURCC_KV[index]));
    if (index <= 2)
    {
        ui->lineEdit->setEnabled(false);
    }
    else if (index == 3) // custom fourCC codec
    {
        ui->lineEdit->setEnabled(true);
    }
}

// Indices:
std::map<int, string> RenderWidMaps::CMB_FOURCC_KV =
{
    {0, "X264"},
    {1, "MP4V"},
    {2, "MJPG"},
    {3,     ""}
};
std::map<string, int> RenderWidMaps::CMB_FOURCC_VK =
{
    {"X264", 0},
    {"MP4V", 1},
    {"MJPG", 2},
    {"",     3}
};
