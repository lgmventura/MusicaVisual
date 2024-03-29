#ifndef CHORDLAYERSETUP_H
#define CHORDLAYERSETUP_H

#include <QDockWidget>
#include <QGridLayout>
#include <QCheckBox>

#include "musiclib/musicdata.h"
#include "musiclib/chords.h"
#include "visual/layercontainer.h"
#include "visual/renderbuffer.h"
#include "ui/colourwidget.h"

namespace Ui {
class ChordLayerSetup;
}

class ChordLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit ChordLayerSetup(MusicData *mdt, LayerContainer *layerCt, RenderBuffer *rBuffer, QWidget *parent = nullptr);
    ~ChordLayerSetup();

    void changeChordLayer(LayerContainer *newLayerCt);
private:
    Ui::ChordLayerSetup *ui;

    LayerContainer *LayerCt;
    MusicData *Mdt;

    RenderBuffer *RBuffer;

    // layout:
    int rowH = 35;

    // UI elements:
    // Tab type/position/general:
    ColourWidget *GridColourWid;
    QWidget *mainWidgetTracks;
    QGridLayout *layoutTracks;
    QWidget *widgetGeneral;
    QGridLayout *layoutGeneral;

    // Tab tracks:
    std::vector<QCheckBox*> *Cb_activeTracks;
    std::vector<ColourWidget*> *Wid_tColours;

    // Tab Chord Star:
    ColourWidget *chStarColourWid;
    QWidget *widgetChordStar;
    QGridLayout *layoutChordStar;

    // functions:
    void drawUi();
    void drawTabTracks();

    void allTracksToggled(bool checked);
    void updateCbAllTracks();

    // Tonnetz
    void calculateTonnetzRadius();

private slots:
    void TrackVisibilityChanged(int track);
    void colourChanged(int track);
    void gridColourChanged();
    void chStColourChanged();

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

    void on_spb_chSt_noteSize_valueChanged(int arg1);

    void on_cb_dispNoteNamesStar_toggled(bool checked);

    void on_cmb_tonnetzShape_currentIndexChanged(int index);

    void on_spb_gridCellSize_valueChanged(int arg1);

    void on_spb_cellSize_valueChanged(int arg1);

    void on_spb_centralMidiPitch_valueChanged(int arg1);

    void on_spb_noteSize_valueChanged(int arg1);

    void on_cb_noteFadeOut_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

signals:
    void changeChordStarTrackActive(int track);
};

#endif // CHORDLAYERSETUP_H
