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
    // initilizing columns:
    ui->tableWidget->insertColumn(0); // For checkboxes
    ui->tableWidget->insertColumn(1); // For names
    ui->tableWidget->insertColumn(2); // For layer type
    ui->tableWidget->insertColumn(3); // For pushbutton edit layer

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

        // inserting layer name:
        this->insertLayerNameLineEdit(iRow, &currLayer);

        // inserting layer type dropdown menu:
        this->insertLayerTypeComboBox(iRow, &currLayer);

        // inserting layer edit push button:
        this->insertLayerSetupPButton(iRow, &currLayer);

    }
    ui->tableWidget->resizeColumnsToContents();
}

void LayerSetup::layerActiveChanged(int layer)
{
    QCheckBox *cb = (QCheckBox*) ui->tableWidget->cellWidget(layer, 0);
    bool state = cb->isChecked();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).LayerActive = state;
}

void LayerSetup::layerNameChanged(int layer)
{
    QLineEdit *ledt = (QLineEdit*) ui->tableWidget->cellWidget(layer, 1);
    QString qName = ledt->text();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).Name = qName.toStdString();
}

void LayerSetup::layerTypeChanged(int layer)
{
    QComboBox *cmbx = (QComboBox*) ui->tableWidget->cellWidget(layer, 2);
    int typeIndex = cmbx->currentIndex();

    std::list<Layer>::iterator it = Layers->begin();
    std::advance(it, layer); // go to layer position
    (*it).LType = Layer::LayerType(typeIndex);
}

void LayerSetup::layerEditTriggered(int layer)
{
    int row = ui->tableWidget->currentRow();

    QComboBox *cmbx = (QComboBox*) ui->tableWidget->cellWidget(row, 2);
    int typeIndex = cmbx->currentIndex();

    std::list<Layer>::iterator it = this->Layers->begin();
    std::advance(it, row); // not it points to the current layer

    if (typeIndex == Layer::LayerType::BlockLayer)
    {
        if (Bls != nullptr)
        {
            Bls->close();
        }
        // set the current layer Bl object and the mainWindow as parent, otherwise, they don't move:
        Bls = new BlockLayerSetup(Mdt, &it->Bl, this->parentWidget());
        Bls->show();
    }
    else if (typeIndex == Layer::LayerType::ChordLayer)
    {
        if (Cls != nullptr)
        {
            Cls->close();
        }
        Cls = new ChordLayerSetup(Mdt, &it->Cl, this->parentWidget());
        Cls->resize(640, 480);
        Cls->show();
    }
}

void LayerSetup::on_pb_addLayer_clicked()
{
    Layer newLayer;
    int row = ui->tableWidget->currentRow();
    if (row == -1) {row = 0;} // if all were deleted, currentRow returns -1
    std::list<Layer>::iterator it = Layers->begin();
    for (int iPos = 0; iPos < row; iPos++) { it++; }
    this->Layers->insert(it, newLayer);
    ui->tableWidget->insertRow(row);

    // creating "blocks active" checkbox:
    this->insertLayerActiveCheckBox(row, &newLayer);

    // inserting layer name:
    this->insertLayerNameLineEdit(row, &newLayer);

    // inserting layer type dropdown menu:
    this->insertLayerTypeComboBox(row, &newLayer);

    // inserting layer edit push button:
    this->insertLayerSetupPButton(row, &newLayer);
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
    cb_layerActive->setStyleSheet("text-align: center; margin-left:44%; margin-right:38%;");
    cb_layerActive->setChecked(currLayer->LayerActive);
    QObject::connect(cb_layerActive, &QCheckBox::toggled, [this, row] { layerActiveChanged(row); });
    //QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
    ui->tableWidget->setCellWidget(row, 0, cb_layerActive);
}

void LayerSetup::insertLayerNameLineEdit(int row, Layer *layer)
{
    std::string layerName = layer->Name;
    QString qLayerName = QString::fromStdString(layerName);
    QLineEdit *le_layerName = new QLineEdit(qLayerName);
    le_layerName->setText(qLayerName);
    QObject::connect(le_layerName, &QLineEdit::editingFinished, [this, row] { layerNameChanged(row); });
    //QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
    ui->tableWidget->setCellWidget(row, 1, le_layerName);
}

void LayerSetup::insertLayerTypeComboBox(int row, Layer *layer)
{
    Layer::LayerType currentType = layer->LType;

    QComboBox *cmb_layerType = new QComboBox();
    cmb_layerType->addItem("Blocks"); // ideally, I should iterate over the enum class, but I would need to define a custom iterator there and so on. Since there is no real plan to add more in the near future, letting it hard-coded here for now.
    cmb_layerType->addItem("Chords");

    cmb_layerType->setCurrentIndex(currentType);
    QObject::connect(cmb_layerType, qOverload<int>(&QComboBox::currentIndexChanged), [this, row] { layerTypeChanged(row); });
    ui->tableWidget->setCellWidget(row, 2, cmb_layerType);
}

void LayerSetup::insertLayerSetupPButton(int row, Layer *layer)
{
    QPushButton *pb_editLayer = new QPushButton();
    QString qPbName = QString::fromStdString("Edit…");
    pb_editLayer->setText(qPbName);

    QObject::connect(pb_editLayer, &QPushButton::clicked, [this, row] { layerEditTriggered(row); });
    ui->tableWidget->setCellWidget(row, 3, pb_editLayer);
}
