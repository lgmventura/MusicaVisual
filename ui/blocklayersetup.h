#ifndef BLOCKLAYERSETUP_H
#define BLOCKLAYERSETUP_H

#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QSpinBox>
#include <QGridLayout>

#include "visual/blocklayers.h"
#include "musicdata.h"
#include "ui/colourwidget.h"

namespace Ui {
class BlockLayerSetup;
}

class BlockLayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit BlockLayerSetup(MusicData *mdt, BlockLayers *tProp, QWidget *parent = nullptr);
    ~BlockLayerSetup();

    // public functions:
    void drawUi();
    void updateUi();

private:
    Ui::BlockLayerSetup *ui;

    BlockLayers *BlockL;
    MusicData *Mdt;

    // layout:
    int rowH = 35;
    QWidget *mainWidget;
    QGridLayout *layout;

    // UI elements:
    std::vector<QCheckBox*> *Cb_trackActive;
    std::vector<ColourWidget*> *Wid_tColours;
    std::vector<QComboBox*> *Cmb_colScheme;
    std::vector<QComboBox*> *Cmb_shape;
    std::vector<QComboBox*> *Cmb_interconnect;
    std::vector<QSpinBox*> *Spb_blur;

    // private functions:
    void allTracksToggled(bool checked);

private slots:
    void trackVisibilityChanged(int track);
    void colourChanged(int track);
    void colourSchemeChanged(int track);
    void shapeChanged(int track);
    void interconnectionsChanged(int track);
    void blurChanged(int track);

    void on_cb_allTracks_stateChanged(int arg1);

signals:
    void changeTrackVisibility(int track);
    //signal for colour change in the ColourWidget class
    void changeColourScheme(int track);
    void changeShape(int track);
    void changeInterconnections(int track);
    void changeBlur(int track);
};

#endif // BLOCKLAYERSETUP_H
