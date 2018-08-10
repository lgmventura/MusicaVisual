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

#include "dockwidrender.h"
#include "ui_dockwidrender.h"
#include "mainwindow.h"

extern RenderP *renderproperties;
extern std::string *codec_fourcc;

DockWidRender::DockWidRender(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidRender)
{
    ui->setupUi(this);
    ui->lineEdit->setText(QString::fromStdString(*codec_fourcc));
    ui->checkBox->setChecked(renderproperties->lines[0]);
    ui->checkBox_2->setChecked(renderproperties->lines[1]);
    ui->checkBox_3->setChecked(renderproperties->sep_render[0]);
    ui->checkBox_4->setChecked(renderproperties->sep_render[1]);
    ui->checkBox_5->setChecked(renderproperties->sep_render[2]);
    ui->spinBox->setValue(renderproperties->beat_measure_manual[0]);
   // ui->comboBox_2->setCurrentIndex(renderproperties->beat_measure_manual[1]);
    ui->spinBox_2->setValue(renderproperties->blur_size[0]);
    ui->spinBox_5->setValue(renderproperties->blur_size[1]);
    ui->spinBox_6->setValue(renderproperties->blur_size_movnotes[0]);
    ui->spinBox_7->setValue(renderproperties->blur_size_movnotes[1]);
    ui->spinBox_3->setValue(renderproperties->vertRange);
    ui->spinBox_4->setValue(renderproperties->vertShift);

    ui->checkBox_6->setChecked(renderproperties->extra_time[0]);
    ui->checkBox_7->setChecked(renderproperties->extra_time[1]);
}

DockWidRender::~DockWidRender()
{
    delete ui;
}

void DockWidRender::on_checkBox_toggled(bool checked)
{
    renderproperties->lines[0] = checked;
}

void DockWidRender::on_checkBox_2_toggled(bool checked)
{
    renderproperties->lines[1] = checked;
}

void DockWidRender::on_spinBox_valueChanged(int arg1)
{
    renderproperties->beat_measure_manual[0] = arg1;
}

void DockWidRender::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    renderproperties->beat_measure_manual[1] = stoi(arg1.toStdString());
}

void DockWidRender::on_checkBox_3_toggled(bool checked)
{
    renderproperties->sep_render[0] = checked;
}

void DockWidRender::on_checkBox_4_toggled(bool checked)
{
    renderproperties->sep_render[1] = checked;
}

void DockWidRender::on_checkBox_5_toggled(bool checked)
{
    renderproperties->sep_render[2] = checked;
}

void DockWidRender::on_spinBox_2_valueChanged(int arg1)
{
    renderproperties->blur_size[0] = arg1;
}

void DockWidRender::on_spinBox_5_valueChanged(int arg1)
{
    renderproperties->blur_size[1] = arg1;
}

void DockWidRender::on_spinBox_6_valueChanged(int arg1)
{
    renderproperties->blur_size_movnotes[0] = arg1;
}

void DockWidRender::on_spinBox_7_valueChanged(int arg1)
{
    renderproperties->blur_size_movnotes[1] = arg1;
}

void DockWidRender::on_lineEdit_editingFinished()
{
    *codec_fourcc = ui->lineEdit->text().toStdString();
}

void DockWidRender::on_spinBox_3_valueChanged(int arg1)
{
    renderproperties->vertRange = arg1;
}

void DockWidRender::on_spinBox_4_valueChanged(int arg1)
{
    renderproperties->vertShift = arg1;
}

void DockWidRender::on_checkBox_6_toggled(bool checked)
{
    renderproperties->extra_time[0] = checked;
}

void DockWidRender::on_checkBox_7_toggled(bool checked)
{
    renderproperties->extra_time[1] = checked;
}
