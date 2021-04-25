#ifndef CHORDLAYERSETUP_H
#define CHORDLAYERSETUP_H

#include <QDockWidget>
#include <QGridLayout>
#include <QCheckBox>

#include "musiclib/musicdata.h"
#include "musiclib/chords.h"
#include "visual/chordlayers.h"

namespace Ui {
class ChordLayerSetup;
}

class ChordLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit ChordLayerSetup(MusicData *mdt, ChordLayers *chordL, QWidget *parent = nullptr);
    ~ChordLayerSetup();

private:
    Ui::ChordLayerSetup *ui;

    ChordLayers *ChordL;
    MusicData *Mdt;

    // layout:
    int rowH = 35;
    QWidget *mainWidgetChordStars;
    QGridLayout *layoutChordStars;

    // UI elements:
    // Tab Chord Star:
    std::vector<QCheckBox*> *Cb_trackActiveChordStars;

    // functions:
    void drawUI();
    void drawTabChordStars();
    // other tabs to be implemented

private slots:
    void chordStarTrackActiveChanged(int track);

signals:
    void changeChordStarTrackActive(int track);
};

#endif // CHORDLAYERSETUP_H
