#include "chordlayersetup.h"
#include "ui_chordlayersetup.h"

ChordLayerSetup::ChordLayerSetup(MusicData *mdt, ChordLayers *chordL, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChordLayerSetup)
{
    ui->setupUi(this);

    this->Mdt = mdt;
    this->ChordL = chordL;

    this->drawUI();
}

ChordLayerSetup::~ChordLayerSetup()
{
    delete ui;
}

void ChordLayerSetup::drawUI()
{
    this->drawTabChordStars();
}

void ChordLayerSetup::drawTabChordStars()
{
    // Setting up layout:
    mainWidgetChordStars = new QWidget(ui->scrollChordStars);
    layoutChordStars = new QGridLayout(mainWidgetChordStars);
    ui->scrollChordStars->setWidget(mainWidgetChordStars);
    mainWidgetChordStars->setLayout(layoutChordStars);
    //mainWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mainWidgetChordStars->setMinimumHeight(rowH*this->Mdt->NTracks);

    // Adding UI elements:
    Cb_trackActiveChordStars = new std::vector<QCheckBox*>;

    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        layoutChordStars->setRowStretch(iTrack, rowH);

        // Add checkbox to set track active state:
        QString trackName = QString::fromStdString(Mdt->TrackNames.at(iTrack));
        QCheckBox *cb_trackActive = new QCheckBox(trackName, mainWidgetChordStars);
//        cb_trackActive->setMinimumHeight(25);
//        cb_trackActive->setMinimumWidth(150);
        cb_trackActive->setChecked(ChordL->ChordStarTrack[iTrack]);
        QObject::connect(cb_trackActive, &QCheckBox::toggled, [this, iTrack] { chordStarTrackActiveChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeChordStarTrackActive(int)), this, SLOT(chordStarTrackActiveChanged(int)));
        Cb_trackActiveChordStars->push_back(cb_trackActive);
        layoutChordStars->addWidget(cb_trackActive, iTrack, 0, 1, 1, Qt::AlignLeft);
    }
}

void ChordLayerSetup::chordStarTrackActiveChanged(int track)
{
    bool state = Cb_trackActiveChordStars->at(track)->isChecked();
    this->ChordL->ChordStarTrack[track] = state;
}
