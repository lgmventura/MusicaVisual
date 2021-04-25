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

#include <algorithm>

#include "ui/trackssetup.h"
#include "ui/blocklayersetup.h"
#include "ui/chordlayersetup.h"
#include "ui/rendersetup.h"
#include "ui/animationbar.h"
#include "ui/about.h"
#include "ui/help.h"

// Include lib classes
#include "midimessages.h"
#include "musicdata.h"
#include "visual/videorecorder.h"
#include "visual/tracks.h"
#include "visual/animation.h"

// From musiclib
#include "chords.h"

#include "opencv2/core.hpp"




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

    void on_actionRendering_Properties_triggered();

    void on_toolButton_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_settings_as_triggered();

    void on_actionLoad_settings_triggered();

    void on_actionAbout_triggered();

    void on_actionHow_does_it_work_triggered();

    void on_actionSqueeze_tracks_triggered();

    void on_pb_process_clicked();

    void on_pb_noteBlocks_clicked();

    void on_pb_animation_clicked();

    void saveSettings(string filePath);

    void loadSettings(string filePath);

    void on_actionSetup_block_layers_triggered();

    void on_actionSetup_chord_layers_triggered();

private:
    Ui::MainWindow *ui;
    BlockLayerSetup *Bls = nullptr;
    ChordLayerSetup *Cls = nullptr;
    RenderWidget *dwrenderprop = nullptr;
    About *aboutDiag = nullptr;
    Help *helpDiag_1 = nullptr;
    MusicData *Mdt = nullptr;
    VideoRecorder *VRec = nullptr;
    TracksP *TProp;
    ChordLayers *ChordL;
    RenderP *RProp;
    AnimState *AState;
    AnimPainter *APainter;
    AnimationBar *AnimBar;
};

#endif // MAINWINDOW_H
