#ifndef LAYERSETUP_H
#define LAYERSETUP_H

#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QScreen> // to make new windows appear on the centre of the main screen

#include "visual/layercontainer.h"
#include "visual/renderbuffer.h"
#include "musicdata.h"
#include "ui/blocklayersetup.h"
#include "ui/chordlayersetup.h"


namespace Ui {
class LayerSetup;
}

class LayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit LayerSetup(std::list<LayerContainer> *layers, MusicData *mdt, RenderBuffer *rBuffer, QWidget *parent = nullptr);
    ~LayerSetup();

    void refresh();

private:
    Ui::LayerSetup *ui;

    QTableWidget *tableWidget;

    std::list<LayerContainer> *Layers;
    MusicData *Mdt;
    RenderBuffer *RBuffer;

    BlockLayerSetup *Bls;
    ChordLayerSetup *Cls;

    QScreen *screen;

    void initUI();

    void insertLayerActiveCheckBox(int row, LayerContainer *layer);
    void insertLayerNameLineEdit(int row, LayerContainer *layer);
    void insertLayerTypeComboBox(int row, LayerContainer *layer);
    void insertLayerSetupPButton(int row, LayerContainer *layer);
    void moveLayer(int fromRow, int toRow);
    void connectTableWidgets();
    void disconnectTabWidgets();


private slots:
    void layerActiveChanged(int layer);
    void layerNameChanged(int layer);
    void layerTypeChanged(int layer);
    void layerEditTriggered(int layer);

    void on_pb_addLayer_clicked();
    void on_pb_cloneLayer_clicked();

    void on_pb_removeLayer_clicked();



    void on_pb_moveUp_clicked();
    void on_pb_moveDown_clicked();
};

#endif // LAYERSETUP_H
