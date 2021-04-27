#include "layersetup.h"
#include "ui_layersetup.h"

LayerSetup::LayerSetup(std::list<Layer> *layers, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LayerSetup)
{
    ui->setupUi(this);

    this->Layers = layers;

    this->initUI();
}

LayerSetup::~LayerSetup()
{
    delete ui;
}

void LayerSetup::initUI()
{
    // initilizing columns:
    ui->tableWidget->insertColumn(0); // For checkboxes
    ui->tableWidget->insertColumn(1); // For names
    ui->tableWidget->insertColumn(2); // For

    // initilizing rows for each layer:
    std::list<Layer>::iterator it = Layers->begin();
    for (int iRow = 0; it != this->Layers->end(); it++, iRow++)
    {
        // getting current layer:
        Layer currLayer = *it;

        // inserting row:
        ui->tableWidget->insertRow(iRow);

        // creating "blocks active" checkbox:
        this->insertLayerActiveCheckBox(iRow, &currLayer);

    }
}

void LayerSetup::layerActiveChanged(int layer)
{
    QCheckBox *cb = (QCheckBox*) ui->tableWidget->cellWidget(layer, 0);
    bool state = cb->isChecked();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).LayerActive = state;
}

void LayerSetup::on_pb_addLayer_clicked()
{
    Layer newLayer;
    int row = ui->tableWidget->currentRow();
    std::list<Layer>::iterator it = Layers->begin();
    for (int iPos = 0; iPos < row; iPos++) { it++; }
    this->Layers->insert(it, newLayer);
    ui->tableWidget->insertRow(row);

    // creating "blocks active" checkbox:
    this->insertLayerActiveCheckBox(row, &newLayer);
}


void LayerSetup::on_pb_removeLayer_clicked()
{
    int row = ui->tableWidget->currentRow();
    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, row);
    this->Layers->erase(it);
    ui->tableWidget->removeRow(row);
}

void LayerSetup::insertLayerActiveCheckBox(int row, Layer *currLayer)
{
    QCheckBox *cb_layerActive = new QCheckBox();
    cb_layerActive->setChecked(currLayer->LayerActive);
    QObject::connect(cb_layerActive, &QCheckBox::toggled, [this, row] { layerActiveChanged(row); });
    //QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
    ui->tableWidget->setCellWidget(row, 0, cb_layerActive);
}
