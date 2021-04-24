#include "blocklayersetup.h"
#include "ui_blocklayersetup.h"


BlockLayerSetup::BlockLayerSetup(MusicData *mdt, TracksP *tProp, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BlockLayerSetup)
{
    ui->setupUi(this);

    Mdt = mdt;
    TProp = tProp;



    // Changing UI elements
    ui->cb_allTracks->setTristate(true);

    this->drawUi();
}

BlockLayerSetup::~BlockLayerSetup()
{
    delete ui;
}

void BlockLayerSetup::drawUi()
{
    // Setting up layout:
    mainWidget = new QWidget(ui->scrollArea);
    layout = new QGridLayout(mainWidget);
    ui->scrollArea->setWidget(mainWidget);
    mainWidget->setLayout(layout);
    //mainWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mainWidget->setMinimumHeight(rowH*this->Mdt->NTracks);

    // Adding UI elements:
    Cb_trackActive = new std::vector<QCheckBox*>;
    Wid_tColours = new std::vector<ColourWidget*>;
    Cmb_colScheme = new std::vector<QComboBox*>;
    Cmb_shape = new std::vector<QComboBox*>;
    Cmb_interconnect = new std::vector<QComboBox*>;
    Spb_blur = new std::vector<QSpinBox*>;
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        layout->setRowStretch(iTrack, rowH);

        // Add checkbox to set track active state:
        QString trackName = QString::fromStdString(Mdt->TrackNames.at(iTrack));
        QCheckBox *cb_trackActive = new QCheckBox(trackName, mainWidget);
//        cb_trackActive->setMinimumHeight(25);
//        cb_trackActive->setMinimumWidth(150);
        cb_trackActive->setChecked(TProp->active[iTrack]);
        QObject::connect(cb_trackActive, &QCheckBox::toggled, [this, iTrack] { trackVisibilityChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeTrackVisibility(int)), this, SLOT(trackVisibilityChanged(int)));
        Cb_trackActive->push_back(cb_trackActive);
        layout->addWidget(cb_trackActive, iTrack, 0, 1, 1, Qt::AlignLeft);


        // Add colour widget:
//        QColor tColour;
//        tColour.setRgb(TProp->getCv(iTrack,0), TProp->getCv(iTrack,1), TProp->getCv(iTrack,2));
//        QWidget *wid_colour = new QWidget(mainWidget);
//        layout->addWidget(wid_colour, iTrack, 1, 1, 1, Qt::AlignLeft);
//        QPalette pal = wid_colour->palette();
//        pal.setColor(QPalette::Window, tColour);
//        wid_colour->setPalette(pal);
//        wid_colour->setAutoFillBackground(true);
        ColourWidget *wid_colour = new ColourWidget(mainWidget);
        wid_colour->setBackgroundColour(TProp->getCv(iTrack,0), TProp->getCv(iTrack,1), TProp->getCv(iTrack,2));
        layout->addWidget(wid_colour, iTrack, 1, 1, 1, Qt::AlignLeft);
        wid_colour->setMinimumHeight(rowH);
        wid_colour->setMinimumWidth(2*rowH);
        Wid_tColours->push_back(wid_colour);
        QObject::connect(wid_colour, &ColourWidget::colourChanged, [this, iTrack] { colourChanged(iTrack); });

        // Add drop down menu for colour scheme:
        QComboBox *cmb_colScheme = new QComboBox(mainWidget);
        int numItems = sizeof(BlockSetupOptions::ColourSchemes)/sizeof(BlockSetupOptions::ColourSchemes[0]);
        for (int iItem = 0; iItem < numItems; iItem++)
        {
            string itemNameStd = BlockSetupOptions::ColourSchemes[iItem];
            QString itemNameQ = QString::fromStdString(itemNameStd);
            cmb_colScheme->addItem(itemNameQ);
        }
        layout->addWidget(cmb_colScheme, iTrack, 2, 1, 1, Qt::AlignLeft);
        cmb_colScheme->setMinimumWidth(4*rowH);
        cmb_colScheme->setCurrentIndex(TProp->colorScheme[iTrack]);
        Cmb_colScheme->push_back(cmb_colScheme);
        QObject::connect(cmb_colScheme, qOverload<int>(&QComboBox::currentIndexChanged), [this, iTrack] { colourSchemeChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeColourScheme(int)), this, SLOT(colourSchemeChanged(int)));


        // Add drop down menu for shapes:
        QComboBox *cmb_shape = new QComboBox(mainWidget);
        numItems = sizeof(BlockSetupOptions::Shapes)/sizeof(BlockSetupOptions::Shapes[0]);
        for (int iItem = 0; iItem < numItems; iItem++)
        {
            string itemNameStd = BlockSetupOptions::Shapes[iItem];
            QString itemNameQ = QString::fromStdString(itemNameStd);
            cmb_shape->addItem(itemNameQ);
        }
        layout->addWidget(cmb_shape, iTrack, 3, 1, 1, Qt::AlignLeft);
        cmb_shape->setMinimumWidth(4*rowH);
        cmb_shape->setCurrentIndex(TProp->shape[iTrack]);
        Cmb_shape->push_back(cmb_shape);
        QObject::connect(cmb_shape, qOverload<int>(&QComboBox::currentIndexChanged), [this, iTrack] { shapeChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeShape(int)), this, SLOT(shapeChanged(int)));

        // Add drop down menu for interconnections:
        QComboBox *cmb_interconnect = new QComboBox(mainWidget);
        numItems = sizeof(BlockSetupOptions::Interconnection)/sizeof(BlockSetupOptions::Interconnection[0]);
        for (int iItem = 0; iItem < numItems; iItem++)
        {
            string itemNameStd = BlockSetupOptions::Interconnection[iItem];
            QString itemNameQ = QString::fromStdString(itemNameStd);
            cmb_interconnect->addItem(itemNameQ);
        }
        layout->addWidget(cmb_interconnect, iTrack, 4, 1, 1, Qt::AlignLeft);
        cmb_interconnect->setMinimumWidth(4*rowH);
        cmb_interconnect->setCurrentIndex(TProp->interconnect[iTrack]);
        Cmb_interconnect->push_back(cmb_interconnect);
        QObject::connect(cmb_interconnect, qOverload<int>(&QComboBox::currentIndexChanged), [this, iTrack] { interconnectionsChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeInterconnections(int)), this, SLOT(interconnectionsChanged(int)));

        // Add dial for blur:
        QDial *dial_blur = new QDial(mainWidget);
        dial_blur->setMaximum(TProp->maxBlur);
        dial_blur->setValue(TProp->track_blur[iTrack]);
        layout->addWidget(dial_blur, iTrack, 5, 1, 1, Qt::AlignLeft);

        // Add spinbox for precise blur:
        QSpinBox *spb_blur = new QSpinBox(mainWidget);
        spb_blur->setMaximum(TProp->maxBlur);
        spb_blur->setValue(TProp->track_blur[iTrack]);
        layout->addWidget(spb_blur, iTrack, 6, 1, 1, Qt::AlignLeft);
        Spb_blur->push_back(spb_blur);
        QObject::connect(spb_blur, qOverload<int>(&QSpinBox::valueChanged), [this, iTrack] { blurChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeBlur(int)), this, SLOT(blurChanged(int)));


        // Connecting dial to spinbox:
        connect(dial_blur, &QDial::valueChanged, spb_blur, &QSpinBox::setValue);
        connect(spb_blur, QOverload<int>::of(&QSpinBox::valueChanged), dial_blur, &QDial::setValue);
    }
}

void BlockLayerSetup::updateUi() // send a signal to all ui elements for them to update
{
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        emit changeTrackVisibility(iTrack);
        // ToDo: colour
        emit changeColourScheme(iTrack);
        emit changeShape(iTrack);
        emit changeInterconnections(iTrack);
        emit changeBlur(iTrack);
    }
}

// slots for UI elements:

void BlockLayerSetup::trackVisibilityChanged(int track)
{
    bool state = Cb_trackActive->at(track)->isChecked();
    this->TProp->active[track] = state;

    // updating checkbox "all tracks" according to how many tracks are visible:
    unsigned int numVisibleTracks = 0;
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        if (this->TProp->active[iTrack] == true)
        {
            numVisibleTracks++;
        }
    }
    if (numVisibleTracks == 0)
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::Unchecked);
    }
    else if (numVisibleTracks == this->Mdt->NTracks)
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::PartiallyChecked);
    }
}

void BlockLayerSetup::colourChanged(int track)
{
    QColor newColour = Wid_tColours->at(track)->getBackgroundColour();
    int r = newColour.red();
    int g = newColour.green();
    int b = newColour.blue();
    this->TProp->setCv(track, 0, r);
    this->TProp->setCv(track, 1, g);
    this->TProp->setCv(track, 2, b);
}

void BlockLayerSetup::colourSchemeChanged(int track)
{
    int newColScheme = Cmb_colScheme->at(track)->currentIndex();
    this->TProp->colorScheme[track] = newColScheme;
}

void BlockLayerSetup::shapeChanged(int track)
{
    int newShapeIndex = Cmb_shape->at(track)->currentIndex();
    this->TProp->shape[track] = newShapeIndex;
}

void BlockLayerSetup::interconnectionsChanged(int track)
{
    int newInterconnIndex = Cmb_interconnect->at(track)->currentIndex();
    this->TProp->interconnect[track] = newInterconnIndex;
}

void BlockLayerSetup::blurChanged(int track)
{
    int newBlurValue = Spb_blur->at(track)->value();
    this->TProp->track_blur[track] = newBlurValue;
}

void BlockLayerSetup::allTracksToggled(bool checked)
{
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        Cb_trackActive->at(iTrack)->setChecked(checked);
        emit changeTrackVisibility(iTrack);
    }
}

void BlockLayerSetup::on_cb_allTracks_stateChanged(int arg1)
{
    bool checked = true;
    if (arg1 == Qt::CheckState::Unchecked)
    {
        checked = false;
    }
    else if (arg1 == Qt::CheckState::Checked)
    {
        checked = true;
    }
    else if (arg1 == Qt::CheckState::PartiallyChecked)
    {
        return;
    }
    this->allTracksToggled(checked);
}
