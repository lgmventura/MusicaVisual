#include "chordlayersetup.h"
#include "ui_chordlayersetup.h"

ChordLayerSetup::ChordLayerSetup(MusicData *mdt, ChordLayers *chordL, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChordLayerSetup)
{
    ui->setupUi(this);

    this->Mdt = mdt;
    this->ChordL = chordL;

    // updating UI elements:
    // Tab Type/Size:
    int numTypes = sizeof(ChordSetupOptions::ChordLayerTypes)/sizeof(ChordSetupOptions::ChordLayerTypes[0]);
    for (int iType = 0; iType < numTypes; iType++)
    {
        QString qItem = QString::fromStdString(ChordSetupOptions::ChordLayerTypes[iType]);
        ui->cb_type->addItem(qItem);
    }
    ui->cb_type->setCurrentIndex(ChordL->CLType);
    ui->dspb_posx->setValue(this->ChordL->x_pos);
    ui->dspb_posy->setValue(this->ChordL->y_pos);
    ui->spb_sizew->setValue(this->ChordL->w);
    ui->spb_sizeh->setValue(this->ChordL->h);

    // Tab tracks:
    this->drawUI();

    // Tab note names:
    ui->cb_sharpFlat->setCurrentIndex((int) ChordL->AccidentalSharp);

    // Tab chord star:
    ui->combox_chordStar->setCurrentIndex(ChordL->ChordStarType);
    ui->spb_chordStarOffset->setValue(ChordL->TurnChordCircle);
    ui->cb_dispNoteNamesStar->setChecked(ChordL->NoteNamesOnStar);
}

ChordLayerSetup::~ChordLayerSetup()
{
    delete ui;
}

void ChordLayerSetup::drawUI()
{
    this->drawTabTracks();
}

void ChordLayerSetup::drawTabTracks()
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

    // updating checkbox "all tracks" according to how many tracks are visible:
    unsigned int numVisibleTracks = 0;
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        if (this->ChordL->ChordStarTrack[iTrack] == true)
        {
            numVisibleTracks++;
        }
    }
    if (numVisibleTracks == 0)
    {
        ui->cb_allTracksChordStars->setCheckState(Qt::CheckState::Unchecked);
    }
    else if (numVisibleTracks == this->Mdt->NTracks)
    {
        ui->cb_allTracksChordStars->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->cb_allTracksChordStars->setCheckState(Qt::CheckState::PartiallyChecked);
    }
}

void ChordLayerSetup::allTracksToggledChordStars(bool checked)
{
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        Cb_trackActiveChordStars->at(iTrack)->setChecked(checked);
        emit changeChordStarTrackActive(iTrack);
    }
}

void ChordLayerSetup::on_cb_allTracksChordStars_stateChanged(int arg1)
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
    this->allTracksToggledChordStars(checked);
}

void ChordLayerSetup::on_dspb_posx_valueChanged(double arg1)
{
    this->ChordL->x_pos = arg1;
}

void ChordLayerSetup::on_dspb_posy_valueChanged(double arg1)
{
    this->ChordL->y_pos = arg1;
}

void ChordLayerSetup::on_spb_sizew_valueChanged(int arg1)
{
    this->ChordL->w = arg1;
}

void ChordLayerSetup::on_spb_sizeh_valueChanged(int arg1)
{
    this->ChordL->h = arg1;
}

void ChordLayerSetup::on_cb_type_currentIndexChanged(int index)
{
    this->ChordL->CLType = ChordLayers::ChordLayerType(index);
}


//void ChordLayerSetup::on_cb_dispChordNames_toggled(bool checked)
//{
//    RProp->chord_names = checked;
//}

//void ChordLayerSetup::on_cb_dispNoteNames_toggled(bool checked)
//{
//    RProp->note_names = checked;
//}

//void ChordLayerSetup::on_cb_dispChordStar_toggled(bool checked)
//{
//    RProp->chord_star = checked;
//}

//void ChordLayerSetup::on_combox_chordStar_currentIndexChanged(int index)
//{
//    if (index == 0)
//    {
//        RProp->chord_star_type = chord::circleOfSemitones;
//    }
//    else if (index == 1)
//    {
//        RProp->chord_star_type = chord::circleOfFifths;
//    }
//}

//void ChordLayerSetup::on_cmb_dispNoteNamesWhere_currentIndexChanged(int index)
//{
//    RProp->note_names_where = index;
//}

//void ChordLayerSetup::on_spb_chordStarOffset_valueChanged(int arg1)
//{
//    RProp->turn_chord_circle = arg1;
//}

//void ChordLayerSetup::on_cb_sharpFlat_currentIndexChanged(int index)
//{
//    if (index == 0)
//        RProp->accidentalSharp = false;
//    else if (index == 1)
//        RProp->accidentalSharp = true;
//}

void ChordLayerSetup::on_cb_sharpFlat_currentIndexChanged(int index)
{
    if (index == 0)
            ChordL->AccidentalSharp = false;
        else if (index == 1)
            ChordL->AccidentalSharp = true;
}

void ChordLayerSetup::on_combox_chordStar_currentIndexChanged(int index)
{
        if (index == 0)
        {
            ChordL->ChordStarType = chord::circleOfSemitones;
        }
        else if (index == 1)
        {
            ChordL->ChordStarType = chord::circleOfFifths;
        }
}

void ChordLayerSetup::on_spb_chordStarOffset_valueChanged(int arg1)
{
    ChordL->TurnChordCircle = arg1;
}

void ChordLayerSetup::on_cb_dispNoteNamesStar_toggled(bool checked)
{
    ChordL->NoteNamesOnStar = checked;
}
