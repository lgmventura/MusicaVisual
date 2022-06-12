#include "chordlayersetup.h"
#include "ui_chordlayersetup.h"

ChordLayerSetup::ChordLayerSetup(MusicData *mdt, LayerContainer *layerCt, RenderBuffer *rBuffer, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChordLayerSetup)
{
    ui->setupUi(this);

    this->Mdt = mdt;
    this->LayerCt = layerCt;

    this->RBuffer = rBuffer;

    this->drawUi();
}

ChordLayerSetup::~ChordLayerSetup()
{
    delete ui;
}

void ChordLayerSetup::changeChordLayer(LayerContainer *newLayerCt)
{
    this->LayerCt = newLayerCt;
    this->drawUi();
}

void ChordLayerSetup::drawUi()
{
    // updating UI elements:
    // Tab Type/Size:
    int cb_type_currIndex = LayerCt->Cl.CLType;
    ui->cb_type->clear();
    int numTypes = sizeof(ChordSetupOptions::ChordLayerTypes)/sizeof(ChordSetupOptions::ChordLayerTypes[0]);
    for (int iType = 0; iType < numTypes; iType++)
    {
        QString qItem = QString::fromStdString(ChordSetupOptions::ChordLayerTypes[iType]);
        ui->cb_type->addItem(qItem);
    }
    ui->cb_type->setCurrentIndex(cb_type_currIndex);
    ui->dspb_posx->setValue(this->LayerCt->Cl.x_pos);
    ui->dspb_posy->setValue(this->LayerCt->Cl.y_pos);
    ui->spb_sizew->setValue(this->LayerCt->Cl.w);
    ui->spb_sizeh->setValue(this->LayerCt->Cl.h);

    widgetGeneral = new QWidget(ui->scrollGeneral);
    layoutGeneral = new QGridLayout(widgetGeneral);
    ui->scrollGeneral->setWidget(widgetGeneral);
    widgetGeneral->setLayout(layoutGeneral);

    QLabel *lb_gridColour = new QLabel;
    QString qstr_gridColour = QString("Grid colour:");
    lb_gridColour->setText(qstr_gridColour);
    layoutGeneral->addWidget(lb_gridColour, 0, 0, 1, 1, Qt::AlignLeft);
    GridColourWid = new ColourWidget(ui->scrollGeneral);
    GridColourWid->setBackgroundColour(LayerCt->Cl.GridColour.r, LayerCt->Cl.GridColour.g, LayerCt->Cl.GridColour.b);
    layoutGeneral->addWidget(GridColourWid, 0, 1, 1, 1, Qt::AlignLeft);
    GridColourWid->setMinimumHeight(30);
    GridColourWid->setMinimumWidth(240);
    GridColourWid->setMaximumHeight(120);
    QObject::connect(GridColourWid, &ColourWidget::colourChanged, [this] { gridColourChanged(); });

    // Tab tracks:
    this->drawTabTracks();

    // Tab note names:
    ui->cb_sharpFlat->setCurrentIndex((int) LayerCt->Cl.AccidentalSharp);

    // Tab chord star:
    ui->combox_chordStar->setCurrentIndex(LayerCt->Cl.ChordStarType);
    ui->spb_chordStarOffset->setValue(LayerCt->Cl.TurnChordCircle);
    ui->cb_dispNoteNamesStar->setChecked(LayerCt->Cl.NoteNamesOnStar);
    ui->spb_chSt_noteSize->setValue(LayerCt->Cl.NoteSize); // using same value from tonnetz, could be changed

    widgetChordStar = new QWidget(ui->scroll_chSt);
    layoutChordStar = new QGridLayout(widgetChordStar);
    ui->scroll_chSt->setWidget(widgetChordStar);
    widgetChordStar->setLayout(layoutChordStar);

    QLabel *lb_ChStColour = new QLabel;
    QString qstr_ChStColour = QString("Chord star colour:");
    lb_ChStColour->setText(qstr_ChStColour);
    layoutChordStar->addWidget(lb_ChStColour, 0, 0, 1, 1, Qt::AlignLeft);
    chStarColourWid = new ColourWidget(ui->scroll_chSt);
    chStarColourWid->setBackgroundColour(LayerCt->Cl.ChordStarColour.r, LayerCt->Cl.ChordStarColour.g, LayerCt->Cl.ChordStarColour.b);
    layoutChordStar->addWidget(chStarColourWid, 0, 1, 1, 1, Qt::AlignLeft);
    chStarColourWid->setMinimumHeight(30);
    chStarColourWid->setMinimumWidth(240);
    chStarColourWid->setMaximumHeight(120);
    QObject::connect(chStarColourWid, &ColourWidget::colourChanged, [this] { chStColourChanged(); });


    // Tab Tonnetz:
    int cmb_tonnetzShape_currIndex = LayerCt->Cl.TonnetzShape;
    ui->cmb_tonnetzShape->clear();
    int numShapes = sizeof(ChordSetupOptions::TonnetzShapes)/sizeof(ChordSetupOptions::TonnetzShapes[0]);
    for (int iShape = 0; iShape < numShapes; iShape++) {
        QString qItem = QString::fromStdString(ChordSetupOptions::TonnetzShapes[iShape]);
        ui->cmb_tonnetzShape->addItem(qItem);
    }
    ui->cmb_tonnetzShape->setCurrentIndex(cmb_tonnetzShape_currIndex);
    ui->spb_centralMidiPitch->setValue(LayerCt->Cl.CentralMidi);
    ui->spb_cellSize->setValue(LayerCt->Cl.CellDiameter);
    ui->spb_gridCellSize->setValue(LayerCt->Cl.HexLayout.size.x);
    ui->spb_noteSize->setValue(LayerCt->Cl.NoteSize);
    this->calculateTonnetzRadius();
}

void ChordLayerSetup::drawTabTracks()
{
    // Setting up layout:
    mainWidgetTracks = new QWidget(ui->scrollTracks);
    layoutTracks = new QGridLayout(mainWidgetTracks);
    ui->scrollTracks->setWidget(mainWidgetTracks);
    mainWidgetTracks->setLayout(layoutTracks);
    //mainWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    mainWidgetTracks->setMinimumHeight(rowH*this->Mdt->NTracks);

    // Adding UI elements:
    Cb_activeTracks = new std::vector<QCheckBox*>;
    Wid_tColours = new std::vector<ColourWidget*>;

    for (unsigned int iTrack = 0; iTrack < this->Mdt->NTracks; iTrack++)
    {
        layoutTracks->setRowStretch(iTrack, rowH);

        // Add checkbox to set track active state:
        QString trackName = QString::fromStdString(Mdt->TrackNames.at(iTrack));
        QCheckBox *cb_trackActive = new QCheckBox(trackName, mainWidgetTracks);
//        cb_trackActive->setMinimumHeight(25);
//        cb_trackActive->setMinimumWidth(150);
        cb_trackActive->setChecked(LayerCt->Cl.ActiveTracks[iTrack]);
        QObject::connect(cb_trackActive, &QCheckBox::toggled, [this, iTrack] { TrackVisibilityChanged(iTrack); });
        QObject::connect(this, SIGNAL(changeChordStarTrackActive(int)), this, SLOT(TrackVisibilityChanged(int)));
        Cb_activeTracks->push_back(cb_trackActive);
        layoutTracks->addWidget(cb_trackActive, iTrack, 0, 1, 1, Qt::AlignLeft);

        // Colour widget:
        ColourWidget *wid_colour = new ColourWidget(mainWidgetTracks);
        wid_colour->setBackgroundColour(LayerCt->getCv(iTrack,0), LayerCt->getCv(iTrack,1), LayerCt->getCv(iTrack,2));
        layoutTracks->addWidget(wid_colour, iTrack, 1, 1, 1, Qt::AlignLeft);
        wid_colour->setMinimumHeight(rowH);
        wid_colour->setMinimumWidth(2*rowH);
        Wid_tColours->push_back(wid_colour);
        QObject::connect(wid_colour, &ColourWidget::colourChanged, [this, iTrack] { colourChanged(iTrack); });
    }

    this->updateCbAllTracks();
}

void ChordLayerSetup::TrackVisibilityChanged(int track)
{
    bool state = Cb_activeTracks->at(track)->isChecked();
    this->LayerCt->Cl.ActiveTracks[track] = state;

    this->updateCbAllTracks();
}


void ChordLayerSetup::colourChanged(int track)
{
    QColor newColour = Wid_tColours->at(track)->getBackgroundColour();
    int r = newColour.red();
    int g = newColour.green();
    int b = newColour.blue();
    this->LayerCt->setCv(track, 0, r);
    this->LayerCt->setCv(track, 1, g);
    this->LayerCt->setCv(track, 2, b);
}

void ChordLayerSetup::gridColourChanged()
{
    QColor newColour = GridColourWid->getBackgroundColour();
    int r = newColour.red();
    int g = newColour.green();
    int b = newColour.blue();
    this->LayerCt->Cl.GridColour.r = r;
    this->LayerCt->Cl.GridColour.g = g;
    this->LayerCt->Cl.GridColour.b = b;
}

void ChordLayerSetup::chStColourChanged()
{
    QColor newColour = chStarColourWid->getBackgroundColour();
    int r = newColour.red();
    int g = newColour.green();
    int b = newColour.blue();
    this->LayerCt->Cl.ChordStarColour.r = r;
    this->LayerCt->Cl.ChordStarColour.g = g;
    this->LayerCt->Cl.ChordStarColour.b = b;
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
        if (this->LayerCt->Cl.ActiveTracks[iTrack] == true)
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
        Cb_activeTracks->at(iTrack)->setChecked(checked);
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
    this->RBuffer->prepareTonnetzGrid(Mdt->PitchMin, Mdt->PitchMax, LayerCt->Cl.CentralMidi);
}

void ChordLayerSetup::on_dspb_posx_valueChanged(double arg1)
{
    this->LayerCt->Cl.x_pos = arg1;
}

void ChordLayerSetup::on_dspb_posy_valueChanged(double arg1)
{
    this->LayerCt->Cl.y_pos = arg1;
}

void ChordLayerSetup::on_spb_sizew_valueChanged(int arg1)
{
    this->LayerCt->Cl.w = arg1;
}

void ChordLayerSetup::on_spb_sizeh_valueChanged(int arg1)
{
    this->LayerCt->Cl.h = arg1;
}

void ChordLayerSetup::on_cb_type_currentIndexChanged(int index)
{
    this->LayerCt->Cl.CLType = ChordLayers::ChordLayerType(index);
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
            LayerCt->Cl.AccidentalSharp = false;
        else if (index == 1)
            LayerCt->Cl.AccidentalSharp = true;
}

void ChordLayerSetup::on_combox_chordStar_currentIndexChanged(int index)
{
        if (index == 0)
        {
            LayerCt->Cl.ChordStarType = Chord::circleOfSemitones;
        }
        else if (index == 1)
        {
            LayerCt->Cl.ChordStarType = Chord::circleOfFifths;
        }
}

void ChordLayerSetup::on_spb_chordStarOffset_valueChanged(int arg1)
{
    LayerCt->Cl.TurnChordCircle = arg1;
}

void ChordLayerSetup::on_cb_dispNoteNamesStar_toggled(bool checked)
{
    LayerCt->Cl.NoteNamesOnStar = checked;
}


void ChordLayerSetup::on_cmb_tonnetzShape_currentIndexChanged(int index)
{
    LayerCt->Cl.TonnetzShape = TonnetzOptions::Shape(index);
}

void ChordLayerSetup::on_spb_gridCellSize_valueChanged(int arg1)
{
    LayerCt->Cl.setTonnetzGridDiameter(arg1);
}

void ChordLayerSetup::on_spb_cellSize_valueChanged(int arg1)
{
    LayerCt->Cl.CellDiameter = arg1;
}

void ChordLayerSetup::on_spb_centralMidiPitch_valueChanged(int arg1)
{
    LayerCt->Cl.CentralMidi = arg1;
    this->calculateTonnetzRadius();
}

void ChordLayerSetup::on_spb_noteSize_valueChanged(int arg1)
{
    LayerCt->Cl.NoteSize = arg1;
}

void ChordLayerSetup::on_spb_chSt_noteSize_valueChanged(int arg1)
{
    LayerCt->Cl.NoteSize = arg1;
}


void ChordLayerSetup::on_cb_noteFadeOut_toggled(bool checked)
{
    LayerCt->Cl.NoteFadeOut = checked;
}

void ChordLayerSetup::on_checkBox_toggled(bool checked)
{
    LayerCt->Cl.NoteCollapse = checked;
}
