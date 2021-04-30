#ifndef LAYERSETUP_H
#define LAYERSETUP_H

#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>

#include "visual/layer.h"
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
    explicit LayerSetup(std::list<Layer> *layers, MusicData *mdt, QWidget *parent = nullptr);
    ~LayerSetup();

private:
    Ui::LayerSetup *ui;

    QTableWidget *tableWidget;

    std::list<Layer> *Layers;
    MusicData *Mdt;

    BlockLayerSetup *Bls;
    ChordLayerSetup *Cls;


    void initUI();

    void insertLayerActiveCheckBox(int row, Layer *layer);
    void insertLayerNameLineEdit(int row, Layer *layer);
    void insertLayerTypeComboBox(int row, Layer *layer);
    void insertLayerSetupPButton(int row, Layer *layer);
    void moveLayer(int fromRow, int toRow);
    void connectTableWidgets();
    void disconnectTabWidgets();


private slots:
    void layerActiveChanged(int layer);
    void layerNameChanged(int layer);
    void layerTypeChanged(int layer);
    void layerEditTriggered(int layer);

    void on_pb_addLayer_clicked();

    void on_pb_removeLayer_clicked();



    void on_pb_moveUp_clicked();
    void on_pb_moveDown_clicked();
};

#endif // LAYERSETUP_H
