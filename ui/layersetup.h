#ifndef LAYERSETUP_H
#define LAYERSETUP_H

#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>

#include "visual/layer.h"


namespace Ui {
class LayerSetup;
}

class LayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit LayerSetup(std::list<Layer*> *layers, QWidget *parent = nullptr);
    ~LayerSetup();

private:
    Ui::LayerSetup *ui;

    std::list<Layer*> *Layers;

    // UI element vectors:
    std::vector<QCheckBox*> Cb_layerActive;
    // not needed for the name, because the lineEdit widget is the standard for QTableWidget
    std::vector<QComboBox*> Cmb_layerType;

    void initUI();

    void insertLayerActiveCheckBox(int row, Layer *layer);
    void removeLayerActiveCheckBox(int row, Layer *layer);


private slots:
    void layerActiveChanged(int layer);

    void on_pb_addLayer_clicked();

    void on_pb_removeLayer_clicked();



};

#endif // LAYERSETUP_H
