#ifndef BLOCKLAYERSETUP_H
#define BLOCKLAYERSETUP_H

#include <QDockWidget>
#include <QSignalMapper>
#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QSpinBox>

#include "visual/tracks.h"
#include "musicdata.h"
#include "ui/colourwidget.h"

namespace Ui {
class BlockLayerSetup;
}

class BlockLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit BlockLayerSetup(MusicData *mdt, TracksP *tProp, QWidget *parent = nullptr);
    ~BlockLayerSetup();

private:
    Ui::BlockLayerSetup *ui;

    TracksP *TProp;
    MusicData *Mdt;

    //QSignalMapper *signalMapper;

    // layout:

    // UI elements:
    std::vector<QCheckBox*> *Cb_trackActive;
    std::vector<ColourWidget*> *Wid_tColours;

private slots:
    void on_cb_track_toggledTriggered(int track);
    void colourChanged(int track);

signals:
    void on_cb_track_toggled(int track);
    void changeColour(int track);
};

#endif // BLOCKLAYERSETUP_H
