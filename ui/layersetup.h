#ifndef LAYERSETUP_H
#define LAYERSETUP_H

#include <QDockWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

#include "visual/layer.h"


namespace Ui {
class LayerSetup;
}

class LayerSetup : public QDockWidget
{
    Q_OBJECT

public:
    explicit LayerSetup(std::list<Layer> *layers, QWidget *parent = nullptr);
    ~LayerSetup();

private:
    Ui::LayerSetup *ui;

    std::list<Layer> *Layers;


    void initUI();

    void insertLayerActiveCheckBox(int row, Layer *layer);
    void insertLayerNameLineEdit(int row, Layer *layer);
    void insertLayerTypeComboBox(int row, Layer *layer);


private slots:
    void layerActiveChanged(int layer);
    void layerNameChanged(int layer);
    void layerTypeChanged(int layer);

    void on_pb_addLayer_clicked();

    void on_pb_removeLayer_clicked();



};

#endif // LAYERSETUP_H
