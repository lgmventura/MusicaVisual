#include "blocklayersetup.h"
#include "ui_blocklayersetup.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QSpinBox>

BlockLayerSetup::BlockLayerSetup(MusicData *mdt, TracksP *tProp, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BlockLayerSetup)
{
    ui->setupUi(this);

    Mdt = mdt;
    TProp = tProp;

    // Setting up layout:
    int rowH = 35;

    QWidget *mainWidget = new QWidget(ui->scrollArea);
    QGridLayout *layout = new QGridLayout(mainWidget);
    ui->scrollArea->setWidget(mainWidget);
    mainWidget->setLayout(layout);
    //mainWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mainWidget->setMinimumHeight(rowH*this->Mdt->NTracks);

    // Adding UI elements:
    std::vector<QCheckBox*> cb_tracksActive;
    std::vector<QWidget*> wid_tColours;
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        layout->setRowStretch(iTrack, rowH);

        // Add checkbox to set track active state:
        QString trackName = QString::fromStdString(Mdt->TrackNames.at(iTrack));
        QCheckBox *cb_trackActive = new QCheckBox(trackName, mainWidget);
//        cb_trackActive->setMinimumHeight(25);
//        cb_trackActive->setMinimumWidth(150);
        cb_trackActive->setChecked(TProp->active[iTrack]);
        cb_tracksActive.push_back(cb_trackActive);
        layout->addWidget(cb_trackActive, iTrack, 0, 1, 1, Qt::AlignLeft);


        // Add colour widget:
        QColor tColour;
        tColour.setRgb(TProp->getCv(iTrack,0), TProp->getCv(iTrack,1), TProp->getCv(iTrack,2));
        QWidget *wid_colour = new QWidget(mainWidget);
        layout->addWidget(wid_colour, iTrack, 1, 1, 1, Qt::AlignLeft);
        QPalette pal = wid_colour->palette();
        pal.setColor(QPalette::Window, tColour);
        wid_colour->setPalette(pal);
        wid_colour->setAutoFillBackground(true);
        wid_colour->setMinimumHeight(rowH);
        wid_colour->setMinimumWidth(2*rowH);
        wid_tColours.push_back(wid_colour);

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

        // Connecting dial to spinbox:
        connect(dial_blur, &QDial::valueChanged, spb_blur, &QSpinBox::setValue);
        connect(spb_blur, QOverload<int>::of(&QSpinBox::valueChanged), dial_blur, &QDial::setValue);
    }
}

BlockLayerSetup::~BlockLayerSetup()
{
    delete ui;
}
