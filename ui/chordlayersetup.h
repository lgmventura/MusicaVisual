#ifndef CHORDLAYERSETUP_H
#define CHORDLAYERSETUP_H

#include <QDockWidget>
#include <QGridLayout>
#include <QCheckBox>

#include "musiclib/musicdata.h"
#include "musiclib/chords.h"
#include "visual/chordlayers.h"
#include "visual/renderbuffer.h"

namespace Ui {
class ChordLayerSetup;
}

class ChordLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit ChordLayerSetup(MusicData *mdt, ChordLayers *chordL, RenderBuffer *rBuffer, QWidget *parent = nullptr);
    ~ChordLayerSetup();

    void changeChordLayer(ChordLayers *newChordL);
private:
    Ui::ChordLayerSetup *ui;

    ChordLayers *ChordL;
    MusicData *Mdt;

    RenderBuffer *RBuffer;

    // layout:
    int rowH = 35;
    QWidget *mainWidgetChordStars;
    QGridLayout *layoutChordStars;

    // UI elements:
    // Tab Chord Star:
    std::vector<QCheckBox*> *Cb_trackActiveChordStars;

    // functions:
    void drawUi();
    void drawTabTracks();

    void allTracksToggled(bool checked);
    void updateCbAllTracks();

    // Tonnetz
    void calculateTonnetzRadius();

private slots:
    void chordStarTrackActiveChanged(int track);

    void on_cb_allTracks_stateChanged(int arg1);

    void on_cb_allTracks_clicked();

    void on_dspb_posx_valueChanged(double arg1);

    void on_dspb_posy_valueChanged(double arg1);

    void on_spb_sizew_valueChanged(int arg1);

    void on_spb_sizeh_valueChanged(int arg1);

    void on_cb_type_currentIndexChanged(int index);

    void on_cb_sharpFlat_currentIndexChanged(int index);

    void on_combox_chordStar_currentIndexChanged(int index);

    void on_spb_chordStarOffset_valueChanged(int arg1);

    void on_cb_dispNoteNamesStar_toggled(bool checked);

    void on_cmb_tonnetzShape_currentIndexChanged(int index);

    void on_spb_gridCellSize_valueChanged(int arg1);

    void on_spb_cellSize_valueChanged(int arg1);

    void on_spb_centralMidiPitch_valueChanged(int arg1);

signals:
    void changeChordStarTrackActive(int track);
};

#endif // CHORDLAYERSETUP_H
