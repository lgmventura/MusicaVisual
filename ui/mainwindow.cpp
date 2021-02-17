/*
 *  This software takes a midi file and creates animations from its notes.
    Copyright (C) 2016  Luiz Guilherme de Medeiros Ventura, Belo Horizonte, Brazil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Very, very, very helpful on working with midi messages: http://midifile.sapp.org/class/MidiMessage/
 * Thanks to Craiggsappmidi (sapp.org) for the library used here.
 * Thanks to OpenCV library used as well to work with images and videos.
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <array>
#include <set>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

//Include library for midi files
#include "importmidi.h"
#include "Options.h"

// Include UI classes
#include "trackssetup.h"
#include "animationbar.h"

// Iostream (just to help on debugging)
#include <iostream>

// Include fstream to save and open objects in .dat
#include <fstream>

// Include QDropEvent to be able to receive file drops
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //TracksP tracksproperties;
    setAcceptDrops(true); // accept file droppings to ease the file opening
    dwidtracks = nullptr; // starting the variables as nullpointer because, if we click load_settings, it will attempt to reopen the widgets if they are open (see load_settings function). But if they had never been instantiated, MusicaVisual will crash.
    dwrenderprop = nullptr;
    this->Mdt = new MusicData(); // create object MusicData
    this->VRec = new VideoRecorder(720, 480, 30); // dimensions, fps etc. will be eventually changed later
    this->TProp = new TracksP();
    this->RProp = new RenderP();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//================================================================================================

void MainWindow::ImportMidiFile(const char *midiFileName)
{
    ImportMidi impMid;
    impMid.importMidiMessagesText(midiFileName);

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(QString::fromStdString(impMid.MidiMessages));
    Mdt->NEvents = impMid.NEvents;
}

void MainWindow::on_actionImport_MIDI_File_triggered()
{
    QString QmidiFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open MIDI File"),
                "//home//",
                "Midi Files (*.mid *.midi);;All Files (*.*)");

    QByteArray ba = QmidiFileName.toUtf8();
    const char *midiFileName = ba.data();

    MainWindow::ImportMidiFile(midiFileName);
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    //if (event->mimeData()->hasFormat("application/x-midi") || event->mimeData()->hasFormat("audio/midi") || event->mimeData()->hasFormat("audio/x-mid")  || event->mimeData()->hasFormat("audio/x-midi") || event->mimeData()->hasFormat("music/crescendo") || event->mimeData()->hasFormat("x-music/x-midi"))
    //{
    QString QmidiFileName;

    QList<QUrl> urls;
    QList<QUrl>::Iterator it;
    urls = event->mimeData()->urls();
    for (it = urls.begin(); it != urls.end(); it++)
        QmidiFileName = it->path();

    QByteArray ba = QmidiFileName.toUtf8();
    const char *midiFileName = ba.data();

    //ui->plainTextEdit->appendPlainText("Dropped midi");
    std::cout << "is midi\n" << midiFileName;
    //ui->plainTextEdit->appendPlainText(QmidiFileName);
    MainWindow::ImportMidiFile(midiFileName);

    event->acceptProposedAction();
    //}
}

void MainWindow::on_actionSave_as_triggered()
{
    QString QmvfFileName = QFileDialog::getSaveFileName(
                this,
                tr("Save MusicaVisual File"),
                "//home//",
                "Musica Visual (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");

    QByteArray ba = QmvfFileName.toUtf8();
    const char *mvfFileName = ba.data();

    if( QmvfFileName.isEmpty()==false )
    {
      QFile scriptFile(QmvfFileName);
      if( !scriptFile.open(QIODevice::WriteOnly) )
      {
        QString msg = QString( tr("Unable to save current script to file: %1") ).arg(QmvfFileName);
        //Logger::GetLogger()->WriteToLog( msg );
        QMessageBox::critical(this, tr("Saving path is blank..."), msg, QMessageBox::Ok );
        return;
      }

      // Save the script
      QTextStream stream(&scriptFile);//   <-----
      stream << ui->plainTextEdit->toPlainText();
      scriptFile.flush();
      scriptFile.close();
//      QTextStream outputStream(&scriptFile);
//      QString code = m_MacroEditor->toPlainText();
//      code.replace("\n","\r\n");
//      outputStream << code;
//      scriptFile.close();
//      SetCurrentMacroFile( fileName );
//      SetMacroChanged( false );
    }
}

void MainWindow::on_actionTracks_triggered() // open dockwidgettracks.
{
    if (Mdt->TrackNames.size() < 24)
        Mdt->nameTracksReset();
    dwidtracks = new TracksWidget(TProp, this, Mdt);
    dwidtracks->show();
    dwidtracks->resize(300,600);
    dwidtracks->pos();
    dwidtracks->repaint();
}

void MainWindow::on_pb_process_clicked() // Process button
{
    std::stringstream stream;
    string str;
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    str = stream.str();
    Mdt->processMidiString(str);
    QMessageBox::information(this, tr("Processing completed"), "The midi data in the text input window was successfully completed.", QMessageBox::Ok );
    ui->pb_noteBlocks->setEnabled(true);
    ui->pb_animation->setEnabled(true);
}



void MainWindow::on_pb_noteBlocks_clicked()
{
    int window_width = ui->spb_res_x->value();
    int window_height = ui->spb_res_y->value();

    cv::Mat *image_win1 = new cv::Mat;
    *image_win1 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    APainter->note_blocks_paint(*image_win1, *Mdt, (char*) "Note blocks", 0, Mdt->TotalTime, window_width, window_height);

    cv::namedWindow("Note blocks");
    cv::imshow("Note blocks", *image_win1);

    cv::waitKey(0);
}



// Animation

void MainWindow::on_pb_animation_clicked()
{
    int window_width = ui->spb_res_x->value();
    int window_height = ui->spb_res_y->value();


    cv::Mat *image_win2 = new cv::Mat;
    *image_win2 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    AState = new AnimState();

    // Since OpenCV 4.x: if you create a vector of cv::Mat objects, you are actually creating a vector of pointers to the SAME matrix. cv::Mat is not a matrix, but rather a pointer to one.
    // First of all, we need to declare the empty matrix:
    cv::Mat mat_zeros = cv::Mat::zeros( window_height, window_width, CV_8UC3 );
    // then we declare the vector of the same type, but without declaring its size yet:
    std::vector <cv::Mat> *img_buffer_sep_tracks = new std::vector <cv::Mat>;
    // and now we use the std::vector.push_back method to insert elements at the end of the vector, doing it for every track (so, I use the for). Then, inside this funcion, I clone the matrix.
    // the method clone() for cv::Mat creates a copy rather than an instance of the matrix. This is how I ensure I am giving having different matrices.
    for (unsigned int kz = 0; kz < Mdt->NTracks; kz++)
    {
        img_buffer_sep_tracks->push_back(mat_zeros.clone());
    }

    cv::namedWindow("Animation");
    AnimBar = new AnimationBar(0, (char*)"Animation", Mdt, image_win2, img_buffer_sep_tracks, window_width, window_height, ui->dsb_fps->value(), RProp, TProp, APainter, AState, VRec);
    AnimBar->show();

    APainter = new AnimPainter();

    if(ui->edt_videoOutput->text().toStdString().size() > 0)
    {
        try
        {
            this->VRec->setVideoDim(window_width, window_height);
            this->VRec->Fps = ui->dsb_fps->value();
            this->VRec->OutputPath = ui->edt_videoOutput->text().toStdString() + "." + this->VRec->getFileExtension();
            // CodecFourCC will be changed directly in object
            this->VRec->createVideoWriter();
            AnimBar->setRecButtonEnabled(true); // video should be ready to be written, activating button
        }
        catch (cv::Exception& e)
        {
            QString msg = QString::fromStdString("Check if the video path and file name is valid and not read only. Check also if the selected fourcc codec (edit->render properties->video codec) is installed in your system. Perhaps you have to install ffmpeg or libavtools.");
            QMessageBox::critical(this, tr("Cannot write video"), msg, QMessageBox::Ok );
        }
    }

    //animwinP->setup(100,200);

    //animbar->findChild<AnimationBar>("on_horizontalSlider_2_valueChanged");
    //animPt->blocks_paint(*image_win2, animwinP->zoom, animwinP->xpos, window_width, window_height);
    //cv::imshow("Animation", *image_win2);

    cv::waitKey(0);
}

//void on_trackbar1(int, void* )
//{
//  zoom = (double) zoom_slider/10 ;

//  note_blocks_paint(*image_win2, "Animation", xpos, (int)total_time/zoom_slider + xpos, ui->spinBox->value(), ui->spinBox_2->value());
//}
//void on_trackbar2(int, void* )
//{
//  xpos = (double) xpos_slider; //beta_slider_max ;

//  //cout << beta << endl;

//  //note_blocks_paint(img, "Animation", startMidiTime + xpos, (int)endMidiTime/zoom_slider + xpos, window_width, window_height);
//  note_blocks_paint(*image_win2, "Animation", xpos, (int)total_time/zoom_slider + xpos, ui->spinBox->value(), ui->spinBox_2->value());
//}

//void MainWindow::on_pushButton_3_clicked()
//{
//    int window_height = ui->spinBox_2->value();
//    int window_width = ui->spinBox->value();

//    cv::Mat *image_win2 = new cv::Mat;
//    *image_win2 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

//    char TrackbarNameA[20];
//    sprintf( TrackbarNameA, "Zoom %d", zoom_slider_max );
//    char TrackbarNameB[20];
//    sprintf( TrackbarNameB, "Position %d", xpos_slider_max );

//    cv::namedWindow("Animation", 1);
//    cv::createTrackbar( TrackbarNameA, "Animation", &zoom_slider, zoom_slider_max, on_trackbar1 );
//    cv::createTrackbar( TrackbarNameB, "Animation", &xpos_slider, xpos_slider_max, on_trackbar2 );
//    on_trackbar1(zoom_slider, 0 );
//    on_trackbar2(xpos_slider, 0 );
//}

void MainWindow::on_actionRendering_Properties_triggered()
{
    dwrenderprop = new RenderWidget(RProp, this, Mdt, VRec);
    dwrenderprop->show();
}

void MainWindow::on_toolButton_clicked()
{
    QFileDialog *qvfsd = new QFileDialog; // qvfsd == Qt Video File Save Dialog
    ui->edt_videoOutput->setText(qvfsd->getSaveFileName(this,
                                                 tr("Save video file"),
                                                 "//home//",
                                                 "Audio and Video Files (*.avi);;Moving Pictures Experts Group 4 (*.mp4 *.mpeg4);;Matroska Video File (*.mkv);;All Files (*.*)"));
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog *opentextfile;
    opentextfile = new QFileDialog();
    QString mvfileName = opentextfile->getOpenFileName(this,
                                                     tr("Open text file"),
                                                     "//home//",
                                                     "Música Visual File (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");
    QFile mvfile(mvfileName);
    if(!mvfile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", mvfile.errorString());
    }

    ui->plainTextEdit->clear();

    QTextStream in(&mvfile);
    //QString line = in.readLine();

    //ui->plainTextEdit->appendPlainText(in.readLine());

    while(!in.atEnd()) {
        QString line;// = in.readLine();
        ui->plainTextEdit->appendPlainText(in.readLine());
    }

    mvfile.close();
}

void MainWindow::on_actionSave_settings_as_triggered()
{
    QFileDialog *dsavesettings;
    dsavesettings = new QFileDialog;
    QString save_file_name = dsavesettings->getSaveFileName(this,
                                                           tr("Save settings"),
                                                           "//home//",
                                                           "Data file (*.dat);;Plain Text File (*.txt);;All Files (*.*)");
    ofstream output_file(save_file_name.toStdString(),ios::binary);
    output_file.write(reinterpret_cast<char*>(TProp),sizeof(*TProp));
    output_file.write(reinterpret_cast<char*>(RProp),sizeof(*RProp));
    output_file.close();
}

void MainWindow::on_actionLoad_settings_triggered()
{
    QFileDialog *dloadsettings;
    dloadsettings = new QFileDialog;
    QString open_file_name = dloadsettings->getOpenFileName(this,
                                                           tr("Load settings"),
                                                           "//home//",
                                                           "Data file (*.dat);;Plain Text File (*.txt);;All Files (*.*)");
    if (open_file_name != nullptr)
    {
        ifstream input_file(open_file_name.toStdString(),ios::binary);
        try
        {
            input_file.read(reinterpret_cast<char*>(TProp),sizeof(*TProp));
            input_file.read(reinterpret_cast<char*>(RProp),sizeof(*RProp));
            cout << "Settings file could be correctly interpreted.";
        }
        catch (exception& e)
        {
            QString msg = QString::fromStdString(std::strcat((char *)"Error on interpreting data from the file. C++ function: input_file.read(reinterpret_cast<char*>(data),sizeof(*data)); Description", e.what()));
            QMessageBox::critical(this, tr("Cannot load settings"), msg, QMessageBox::Ok );
        }
        catch (...)
        {
            QString msg = QString::fromStdString("Unexpected error on calling: input_file.read(reinterpret_cast<char*>(data),sizeof(*data));");
            QMessageBox::critical(this, tr("Cannot load settings"), msg, QMessageBox::Ok );
        }
        input_file.close();
        if (dwidtracks != nullptr)
        {
            dwidtracks->close();
            dwidtracks = new TracksWidget(TProp, this, Mdt);
            dwidtracks->show();
        }
        if (dwrenderprop != nullptr)
        {
            dwrenderprop->close();
            dwrenderprop = new RenderWidget(RProp, this, Mdt, VRec);
            dwrenderprop->show();
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    //if (aboutDiag == nullptr)
    {
        aboutDiag = new About(this);
        aboutDiag->show();
    }
}

void MainWindow::on_actionHow_does_it_work_triggered()
{
    {
        QFile helpFile(":doc/en/help_en.html");
//        stringstream helpStrStream;
//        string line;
//        ifstream helpFile ();
//        if (helpFile.is_open())
//        {
//            while ( getline (helpFile, line))
//            {
//                helpStrStream << line << endl;
//                cout << line << endl;
//            }
//            helpFile.close();
//        }
//        else {
//            cout << "Help file not found." << endl;
//        }
        if (!helpFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        string helpStr = "";

        QTextStream in(&helpFile);
        QString line = in.readLine();
        while (!line.isNull()) {
            helpStr = helpStr + line.toStdString() + '\n';
            line = in.readLine();
        }
        //cout << helpStr << endl;
        helpDiag_1 = new Help(helpStr, this);
        helpDiag_1->show();
    }
}

//void print_set(aList) //only for debugging
//{
//    for (A const& a : aList)
//    {
//        std::cout << a << ' ';
//    }
//}

void MainWindow::on_actionSqueeze_tracks_triggered()
{
    std::stringstream stream; // for the whole text in the edit area
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    std::string midiMessagesFromTextEdt = stream.str();
    std::string midiMessagesSqz = Mdt->squeezeTracksMidiStr(midiMessagesFromTextEdt);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(QString::fromStdString(midiMessagesSqz));
}
