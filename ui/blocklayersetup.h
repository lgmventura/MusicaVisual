#ifndef BLOCKLAYERSETUP_H
#define BLOCKLAYERSETUP_H

#include <QDockWidget>
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

    // layout:

    // UI elements:
    std::vector<QCheckBox*> *Cb_trackActive;
    std::vector<ColourWidget*> *Wid_tColours;
    std::vector<QComboBox*> *Cmb_colScheme;
    std::vector<QComboBox*> *Cmb_shape;
    std::vector<QComboBox*> *Cmb_interconnect;
    std::vector<QSpinBox*> *Spb_blur;

private slots:
    void on_cb_track_toggledTriggered(int track);
    void colourChanged(int track);
    void colourSchemeChanged(int track);
    void shapeChanged(int track);
    void interconnectionsChanged(int track);
    void blurChanged(int track);

signals:
    void on_cb_track_toggled(int track);
    //signal for colour change in the ColourWidget class
    void changeColourScheme(int track);
    void changeShape(int track);
    void changeInterconnections(int track);
    void changeBlur(int track);
};

#endif // BLOCKLAYERSETUP_H
