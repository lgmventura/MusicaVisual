#include "layersetup.h"
#include "ui_layersetup.h"

LayerSetup::LayerSetup(std::list<Layer> *layers, MusicData *mdt, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LayerSetup)
{
    ui->setupUi(this);

    this->Layers = layers;
    this->Mdt = mdt;
    this->Bls = nullptr;
    this->Cls = nullptr;

    this->initUI();
}

LayerSetup::~LayerSetup()
{
    delete ui;
}

void LayerSetup::initUI()
{
    // creating tableWidget:
    this->tableWidget = new QTableWidget();
    ui->gridLayout->addWidget(this->tableWidget, 0, 1, 0, -1);

    // initilizing columns:
    this->tableWidget->insertColumn(0); // For checkboxes
    this->tableWidget->insertColumn(1); // For names
    this->tableWidget->insertColumn(2); // For layer type
    this->tableWidget->insertColumn(3); // For pushbutton edit layer

    // initilizing rows for each layer:
    std::list<Layer>::iterator it = Layers->begin();
    for (int iRow = 0; it != this->Layers->end(); it++, iRow++)
    {
        // getting current layer:
        Layer currLayer = *it;

        // inserting row:
        this->tableWidget->insertRow(iRow);

        // creating "blocks active" checkbox:
        this->insertLayerActiveCheckBox(iRow, &currLayer);

        // inserting layer name:
        this->insertLayerNameLineEdit(iRow, &currLayer);

        // inserting layer type dropdown menu:
        this->insertLayerTypeComboBox(iRow, &currLayer);

        // inserting layer edit push button:
        this->insertLayerSetupPButton(iRow, &currLayer);

    }
    this->tableWidget->resizeColumnsToContents();
    this->connectTableWidgets();

//    this->tableWidget->setDragEnabled(true);
//    this->tableWidget->setAcceptDrops(true);
//    this->tableWidget->viewport()->setAcceptDrops(true);
//    this->tableWidget->setDragDropOverwriteMode(false);
//    this->tableWidget->setDropIndicatorShown(true);

//    this->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
//    this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
//    this->tableWidget->setDragDropMode(QAbstractItemView::InternalMove);
}

void LayerSetup::layerActiveChanged(int layer)
{
    QCheckBox *cb = (QCheckBox*) this->tableWidget->cellWidget(layer, 0);
    bool state = cb->isChecked();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).LayerActive = state;
}

void LayerSetup::layerNameChanged(int layer)
{
    QLineEdit *ledt = (QLineEdit*) this->tableWidget->cellWidget(layer, 1);
    QString qName = ledt->text();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).setName(qName.toStdString());
}

void LayerSetup::layerTypeChanged(int layer)
{
    QComboBox *cmbx = (QComboBox*) this->tableWidget->cellWidget(layer, 2);
    int typeIndex = cmbx->currentIndex();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).LType = Layer::LayerType(typeIndex);
}

void LayerSetup::layerEditTriggered(int layer)
{
    int row = this->tableWidget->currentRow();

    QComboBox *cmbx = (QComboBox*) this->tableWidget->cellWidget(row, 2);
    int typeIndex = cmbx->currentIndex();

    std::list<Layer>::iterator it = this->Layers->begin();
    std::advance(it, row); // not it points to the current layer

    if (typeIndex == Layer::LayerType::BlockLayer)
    {
        if (Bls == nullptr)
        {
            Bls = new BlockLayerSetup(Mdt, &it->Bl, this->parentWidget());
        }
        else
        {
            Bls->changeBlockLayer(&it->Bl);
        }
        // set the current layer Bl object and the mainWindow as parent, otherwise, they don't move:
        QString qWindowTitle = "Block layer setup for " + QString::fromStdString((*it).getName());
        Bls->setWindowTitle(qWindowTitle);
        Bls->show();
    }
    else if (typeIndex == Layer::LayerType::ChordLayer)
    {
        if (Cls == nullptr)
        {
            Cls = new ChordLayerSetup(Mdt, &it->Cl, this->parentWidget());
            Cls->resize(640, 480);
        }
        else
        {
            Cls->changeChordLayer(&it->Cl);
        }

        QString qWindowTitle = "Chord layer setup for " + QString::fromStdString((*it).getName());
        Cls->setWindowTitle(qWindowTitle);
        Cls->show();
    }
}

void LayerSetup::on_pb_addLayer_clicked()
{
    Layer newLayer;
    int numLayers = Layers->size();
    newLayer.setName("Layer " + std::to_string(numLayers));
    int row = this->tableWidget->currentRow();
    if (row == -1) {row = 0;} // if all were deleted, currentRow returns -1
    std::list<Layer>::iterator it = Layers->begin();
    for (int iPos = 0; iPos < row; iPos++) { it++; }
    this->Layers->insert(it, newLayer);
    this->tableWidget->insertRow(row);

    // creating "blocks active" checkbox:
    this->insertLayerActiveCheckBox(row, &newLayer);

    // inserting layer name:
    this->insertLayerNameLineEdit(row, &newLayer);

    // inserting layer type dropdown menu:
    this->insertLayerTypeComboBox(row, &newLayer);

    // inserting layer edit push button:
    this->insertLayerSetupPButton(row, &newLayer);

    // connecting:
    this->connectTableWidgets();
}


void LayerSetup::on_pb_removeLayer_clicked()
{
    int row = this->tableWidget->currentRow();
    if (row >= 0)
    {
        std::list<Layer>::iterator it = Layers->begin();
        std::advance(it, row);
        this->Layers->erase(it);
        this->tableWidget->removeRow(row);
        this->disconnectTabWidgets();
        this->connectTableWidgets();
    }
}

void LayerSetup::insertLayerActiveCheckBox(int row, Layer *layer)
{
    QCheckBox *cb_layerActive = new QCheckBox();
    cb_layerActive->setStyleSheet("text-align: center; margin-left:44%; margin-right:38%;");
    cb_layerActive->setChecked(layer->LayerActive);

    //QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
    this->tableWidget->setCellWidget(row, 0, cb_layerActive);
}

void LayerSetup::insertLayerNameLineEdit(int row, Layer *layer)
{
    std::string layerName = layer->getName();
    QString qLayerName = QString::fromStdString(layerName);
    QLineEdit *le_layerName = new QLineEdit(qLayerName);
    le_layerName->setText(qLayerName);

    //QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
    this->tableWidget->setCellWidget(row, 1, le_layerName);
}

void LayerSetup::insertLayerTypeComboBox(int row, Layer *layer)
{
    Layer::LayerType currentType = layer->LType;

    QComboBox *cmb_layerType = new QComboBox();
    cmb_layerType->addItem("Blocks"); // ideally, I should iterate over the enum class, but I would need to define a custom iterator there and so on. Since there is no real plan to add more in the near future, letting it hard-coded here for now.
    cmb_layerType->addItem("Chords");

    cmb_layerType->setCurrentIndex(currentType);

    this->tableWidget->setCellWidget(row, 2, cmb_layerType);
}

void LayerSetup::insertLayerSetupPButton(int row, Layer *layer)
{
    QPushButton *pb_editLayer = new QPushButton();
    QString qPbName = QString::fromStdString("Edit…");
    pb_editLayer->setText(qPbName);


    this->tableWidget->setCellWidget(row, 3, pb_editLayer);
}

void LayerSetup::moveLayer(int fromRow, int toRow)
{
    std::list<Layer>::iterator it = this->Layers->begin();
    std::advance(it, fromRow);
    Layer movedLayer = (*it);
    this->Layers->erase(it);
    it = this->Layers->begin();
    std::advance(it, toRow);
    this->Layers->insert(it, movedLayer);

    this->refresh();
    this->tableWidget->selectRow(toRow);
}


void LayerSetup::connectTableWidgets()
{
    std::list<Layer>::iterator it = Layers->begin();
    for (int iRow = 0; it != this->Layers->end(); it++, iRow++)
    {
        // getting widgets:
        QCheckBox *cb = (QCheckBox*) this->tableWidget->cellWidget(iRow, 0);
        QLineEdit *ledt = (QLineEdit*) this->tableWidget->cellWidget(iRow, 1);
        QComboBox *cmbx = (QComboBox*) this->tableWidget->cellWidget(iRow, 2);
        QPushButton *pb = (QPushButton*) this->tableWidget->cellWidget(iRow, 3);

        // connecting them:
        QObject::connect(cb, &QCheckBox::toggled, [this, iRow] { layerActiveChanged(iRow); });
        QObject::connect(ledt, &QLineEdit::editingFinished, [this, iRow] { layerNameChanged(iRow); });
        QObject::connect(cmbx, qOverload<int>(&QComboBox::currentIndexChanged), [this, iRow] { layerTypeChanged(iRow); });
        QObject::connect(pb, &QPushButton::clicked, [this, iRow] { layerEditTriggered(iRow); });
    }
}

void LayerSetup::disconnectTabWidgets()
{
    std::list<Layer>::iterator it = Layers->begin();
    for (int iRow = 0; it != this->Layers->end(); it++, iRow++)
    {
        // getting widgets:
        QCheckBox *cb = (QCheckBox*) this->tableWidget->cellWidget(iRow, 0);
        QLineEdit *ledt = (QLineEdit*) this->tableWidget->cellWidget(iRow, 1);
        QComboBox *cmbx = (QComboBox*) this->tableWidget->cellWidget(iRow, 2);
        QPushButton *pb = (QPushButton*) this->tableWidget->cellWidget(iRow, 3);

        // connecting them:
        QObject::disconnect(cb, 0,0,0);
        QObject::disconnect(ledt, 0,0,0);
        QObject::disconnect(cmbx, 0,0,0);
        QObject::disconnect(pb, 0,0,0);
    }
}

void LayerSetup::on_pb_moveUp_clicked()
{
    int currRow = this->tableWidget->currentRow();
    if (currRow > 0)
    {
        this->moveLayer(currRow, currRow-1);
    }
}

void LayerSetup::on_pb_moveDown_clicked()
{
    int currRow = this->tableWidget->currentRow();
    int numRows = this->tableWidget->rowCount();
    if (currRow >= 0 && currRow < numRows - 1)
    {
        this->moveLayer(currRow, currRow+1);
    }
}


void LayerSetup::refresh()
{
    this->disconnectTabWidgets();
    for (int iRow = 0; iRow < this->tableWidget->rowCount(); iRow++)
    {
        this->tableWidget->removeRow(iRow);
    }
    this->initUI();
}
