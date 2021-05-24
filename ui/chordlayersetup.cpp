#include "chordlayersetup.h"
#include "ui_chordlayersetup.h"

ChordLayerSetup::ChordLayerSetup(MusicData *mdt, ChordLayers *chordL, RenderBuffer *rBuffer, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChordLayerSetup)
{
    ui->setupUi(this);

    this->Mdt = mdt;
    this->ChordL = chordL;

    this->RBuffer = rBuffer;

    this->drawUi();
}

ChordLayerSetup::~ChordLayerSetup()
{
    delete ui;
}

void ChordLayerSetup::changeChordLayer(ChordLayers *newChordL)
{
    this->ChordL = newChordL;
    this->drawUi();
}

void ChordLayerSetup::drawUi()
{
    // updating UI elements:
    // Tab Type/Size:
    int cb_type_currIndex = ChordL->CLType;
    ui->cb_type->clear();
    int numTypes = sizeof(ChordSetupOptions::ChordLayerTypes)/sizeof(ChordSetupOptions::ChordLayerTypes[0]);
    for (int iType = 0; iType < numTypes; iType++)
    {
        QString qItem = QString::fromStdString(ChordSetupOptions::ChordLayerTypes[iType]);
        ui->cb_type->addItem(qItem);
    }
    ui->cb_type->setCurrentIndex(cb_type_currIndex);
    ui->dspb_posx->setValue(this->ChordL->x_pos);
    ui->dspb_posy->setValue(this->ChordL->y_pos);
    ui->spb_sizew->setValue(this->ChordL->w);
    ui->spb_sizeh->setValue(this->ChordL->h);

    // Tab tracks:
    this->drawTabTracks();

    // Tab note names:
    ui->cb_sharpFlat->setCurrentIndex((int) ChordL->AccidentalSharp);

    // Tab chord star:
    ui->combox_chordStar->setCurrentIndex(ChordL->ChordStarType);
    ui->spb_chordStarOffset->setValue(ChordL->TurnChordCircle);
    ui->cb_dispNoteNamesStar->setChecked(ChordL->NoteNamesOnStar);

    // Tab Tonnetz:
    int cmb_tonnetzShape_currIndex = ChordL->TonnetzShape;
    ui->cmb_tonnetzShape->clear();
    int numShapes = sizeof(ChordSetupOptions::TonnetzShapes)/sizeof(ChordSetupOptions::TonnetzShapes[0]);
    for (int iShape = 0; iShape < numShapes; iShape++) {
        QString qItem = QString::fromStdString(ChordSetupOptions::TonnetzShapes[iShape]);
        ui->cmb_tonnetzShape->addItem(qItem);
    }
    ui->cmb_tonnetzShape->setCurrentIndex(cmb_tonnetzShape_currIndex);
    ui->spb_centralMidiPitch->setValue(ChordL->CentralMidi);
    ui->spb_cellSize->setValue(ChordL->CellDiameter);
    ui->spb_gridCellSize->setValue(ChordL->HexLayout.size.x);
    ui->spb_noteSize->setValue(ChordL->NoteSize);
    this->calculateTonnetzRadius();
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
        cb_trackActive->setChecked(ChordL->ActiveTracks[iTrack]);
        QObject::connect(cb_trackActive, &QCheckBox::toggled, [this, iTrack] { chordStarTrackActiveChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeChordStarTrackActive(int)), this, SLOT(chordStarTrackActiveChanged(int)));
        Cb_trackActiveChordStars->push_back(cb_trackActive);
        layoutChordStars->addWidget(cb_trackActive, iTrack, 0, 1, 1, Qt::AlignLeft);
    }
    this->updateCbAllTracks();
}

void ChordLayerSetup::chordStarTrackActiveChanged(int track)
{
    bool state = Cb_trackActiveChordStars->at(track)->isChecked();
    this->ChordL->ActiveTracks[track] = state;

    this->updateCbAllTracks();
}

void ChordLayerSetup::on_cb_allTracks_clicked()
{
    if (ui->cb_allTracks->checkState() == Qt::CheckState::PartiallyChecked)
    {
        this->allTracksToggled(true); // skipping state partially checked when clicking the check box
    }
}

void ChordLayerSetup::updateCbAllTracks()
{
    // updating checkbox "all tracks" according to how many tracks are visible:
    unsigned int numVisibleTracks = 0;
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        if (this->ChordL->ActiveTracks[iTrack] == true)
        {
            numVisibleTracks++;
        }
    }
    if (numVisibleTracks == 0) // no visible tracks
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::Unchecked);
    }
    else if (numVisibleTracks == this->Mdt->NTracks) // all tracks visible
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::Checked);
    }
    else // any number in between 0 and N of tracks visible
    {
        ui->cb_allTracks->setCheckState(Qt::CheckState::PartiallyChecked);
    }
}

void ChordLayerSetup::allTracksToggled(bool checked)
{
    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        Cb_trackActiveChordStars->at(iTrack)->setChecked(checked);
        emit changeChordStarTrackActive(iTrack);
    }
}

void ChordLayerSetup::on_cb_allTracks_stateChanged(int arg1)
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

void ChordLayerSetup::calculateTonnetzRadius()
{
    this->RBuffer->prepareTonnetzGrid(Mdt->PitchMin, Mdt->PitchMax, ChordL->CentralMidi);
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
            ChordL->ChordStarType = Chord::circleOfSemitones;
        }
        else if (index == 1)
        {
            ChordL->ChordStarType = Chord::circleOfFifths;
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


void ChordLayerSetup::on_cmb_tonnetzShape_currentIndexChanged(int index)
{
    ChordL->TonnetzShape = TonnetzOptions::Shape(index);
}

void ChordLayerSetup::on_spb_gridCellSize_valueChanged(int arg1)
{
    ChordL->setTonnetzGridDiameter(arg1);
}

void ChordLayerSetup::on_spb_cellSize_valueChanged(int arg1)
{
    ChordL->CellDiameter = arg1;
}

void ChordLayerSetup::on_spb_centralMidiPitch_valueChanged(int arg1)
{
    ChordL->CentralMidi = arg1;
    this->calculateTonnetzRadius();
}

void ChordLayerSetup::on_spb_noteSize_valueChanged(int arg1)
{
    ChordL->NoteSize = arg1;
}


void ChordLayerSetup::on_cb_noteFadeOut_toggled(bool checked)
{
    ChordL->NoteFadeOut = checked;
}

void ChordLayerSetup::on_checkBox_toggled(bool checked)
{
    ChordL->NoteCollapse = checked;
}
