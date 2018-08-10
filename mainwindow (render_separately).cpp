/*
 *
 *
 * Very, very, very helpful on working with midi messages: http://midifile.sapp.org/class/MidiMessage/
 * Thanks to Craiggsappmidi (sapp.org) for the library used here.
 * Thanks to OpenCV library used as well to work with images and videos.
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <array>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

//Include library for midi files
#include "MidiFile.h"
#include "Options.h"

#include "dockwidgettracks.h"
#include "midimessages.h"
#include "animationbar.h"

//Include OpenCV image processing libraries
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

// Iostream (just to help on debugging)
#include <iostream>

using namespace std;

TracksP::TracksP()
{

}
RenderP::RenderP()
{

}

TracksP *tracksproperties = new TracksP(); // Global variable with tracks parameters for exhibition.
int n_events; // Global var to store number of events
std::list <MidiNote> notes; // List of processed notes.
std::list <TempoChange> *tempos = new std::list <TempoChange>;
unsigned long total_time = 0; // Global variable to store the total time of the current processed midi file.
unsigned int pitch_max = 1, pitch_min = 100;
unsigned int tracks_count = 1;
unsigned int tpq; // Ticks per Quarter-Note

AnimationBar *animbar;
AnimwinP *animwinP;
AnimPainter *animPt;

RenderP *renderproperties = new RenderP();

//cv::Point pt3,pt4; // Points for the central line

void AnimPainter::blocks_paint(cv::Mat image, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2;
    double x1, x2, y1, y2;
    int x_max = window_width + 5000, y_max = window_height + 50;
    //std::cout << "Paint blocks! " << pitch_min << ' ' << pitch_max << endl;
    cv::Mat img_playing_notes = cv::Mat::zeros( window_height, window_width, CV_8UC3 );; // only if if (renderproperties->sep_render[0] == true)
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        x1 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_on)/((double)endMidiTime - (double)startMidiTime));
        x2 = (double)window_width*((-(double)startMidiTime + (double)(*it).t_off)/((double)endMidiTime - (double)startMidiTime));
        y1 = (double)window_height - (double)window_height*((double)(*it).pitch/((double)pitch_max - (double)pitch_min + 20.0));
        y2 = (double)window_height - (double)window_height*(((double)(*it).pitch + 1.0)/((double)pitch_max - (double)pitch_min + 20.0));
        pt1.x = (int)(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
        pt2.x = (int)(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
        pt1.y = (int)(y1); //window_height*((*it).pitch/(50));
        pt2.y = (int)(y2);//window_height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
        if (std::abs(pt1.x) < x_max && std::abs(pt1.y) < y_max && std::abs(pt2.x) < x_max && std::abs(pt2.y) < y_max) // This "if" is just to bypass an issue (certainly about big doubles being coverted to int), what causes random boxes appearing on the screen when one zooms in very close.
        {
            for (short tnum = 0; tnum < 15; tnum++)
            {
                if ((*it).track == tnum+1 && tracksproperties->active[tnum] == true) // All tracks - rectangle
                {
                    if (pt1.x > window_width/2) // The note block is before (to the right of) the center line
                        cv::rectangle( image, pt1, pt2, {tracksproperties->getCv(tnum,2)*(*it).vel*3/512,  tracksproperties->getCv(tnum,1)*(*it).vel*3/512,  tracksproperties->getCv(tnum,0)*(*it).vel*3/512}, -1, 8 );
                    else if (pt1.x <= window_width/2 && pt2.x > window_width/2) // The note block is inside the center line
                    {
                        if ( !renderproperties->sep_render[0]) // Only if the playing notes are to to rendered together
                            cv::rectangle( image, pt1, pt2, {tracksproperties->getCv(tnum,2)*(*it).vel/128,  tracksproperties->getCv(tnum,1)*(*it).vel/128,  tracksproperties->getCv(tnum,0)*(*it).vel/128}, -1, 8 );
                        else
                            cv::rectangle( img_playing_notes, pt1, pt2, {tracksproperties->getCv(tnum,2)*(*it).vel/128,  tracksproperties->getCv(tnum,1)*(*it).vel/128,  tracksproperties->getCv(tnum,0)*(*it).vel/128}, -1, 8 );
                    }
                    else // The note block is after (to the left of) the center line
                        cv::rectangle( image, pt1, pt2, {tracksproperties->getCv(tnum,2)*(*it).vel/512,  tracksproperties->getCv(tnum,1)*(*it).vel/512,  tracksproperties->getCv(tnum,0)*(*it).vel/512}, 2, 8 );
                }
            }
            if ( renderproperties->sep_render[0])
                image = image + img_playing_notes;
        }
    }
//    pt3.x = window_width/2;
//    pt4.x = window_width/2;
//    pt3.y = window_height;
//    pt4.y = 0;
    if (renderproperties->lines[0])
        cv::line(image, cv::Point(window_width/2, window_height), cv::Point(window_width/2, 0), {100,100,100});
    if (renderproperties->lines[1])
    {
        for (unsigned int i = 0; i < endMidiTime; i = i + renderproperties->beat_measure_manual[0]*tpq/renderproperties->beat_measure_manual[1])
        {
            cv::line(image, cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), window_height), cv::Point((int)((double)window_width*((-(double)startMidiTime + (double)i)/((double)endMidiTime - (double)startMidiTime))), 0), {60,60,60});
        }
    }

}

//================================================================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //TracksP tracksproperties;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImport_MIDI_File_triggered()
{
    QString QmidiFileName = QFileDialog::getOpenFileName(
                this,
                tr("Open MIDI File"),
                "//home//",
                "Midi Files (*.mid *.midi);;All Files (*.*)");

    QByteArray ba = QmidiFileName.toLatin1();
    const char *midiFileName = ba.data();

//    Options options;
//       options.process(midiFileName);//(argc, argv);
//       MidiFile midifile;
//       if (options.getArgCount() > 0) {
//          midifile.read(options.getArg(1));
//       } else {
//          midifile.read(cin);
//       }
    MidiFile midifile;
    midifile.read(midiFileName);

    ui->plainTextEdit->clear(); // unless you know the editor is empty
    //ui->plainTextEdit->appendPlainText(midiFileName);


//        ui->plainTextEdit->appendPlainText("TPQ: ");
//        ui->plainTextEdit->appendPlainText(QString::number(midifile.getTicksPerQuarterNote()));
//        ui->plainTextEdit->appendPlainText("TRACKS: ");
//        ui->plainTextEdit->appendPlainText(QString::number(midifile.getTrackCount()));
       std::stringstream buffer;
       buffer << "TPQ: " << midifile.getTicksPerQuarterNote() << endl; // Get the Ticks Per QarterNote (TPQ), necessary for setting the tempo
       buffer << "TRACKS: " << midifile.getTrackCount() << endl;

       midifile.joinTracks();
       // midifile.getTrackCount() will now return "1", but original
       // track assignments can be seen in .track field of MidiEvent.

//       ui->plainTextEdit->appendPlainText("TICK    DELTA   TRACK   MIDI MESSAGE\n");
//       ui->plainTextEdit->appendPlainText("____________________________________\n");
       buffer << "TICK    DELTA   TRACK   MIDI MESSAGE\n";
       buffer << "____________________________________\n";
       ui->plainTextEdit->appendPlainText(QString::fromStdString(buffer.str()));

       MidiEvent* mev;
       int deltatick;
       for (int event=0; event < midifile[0].size(); event++) {
          mev = &midifile[0][event];
          if (event == 0) {
             deltatick = mev->tick;
          } else {
             deltatick = mev->tick - midifile[0][event-1].tick;
          }
//          ui->plainTextEdit->appendPlainText(QString::number(qint64(dec)));
//          ui->plainTextEdit->appendPlainText(QString::number(mev->tick));
//          ui->plainTextEdit->appendPlainText("\t");
//          ui->plainTextEdit->appendPlainText(QString::number(deltatick));
//          ui->plainTextEdit->appendPlainText("\t");
//          ui->plainTextEdit->appendPlainText(QString::number(mev->track));
//          ui->plainTextEdit->appendPlainText("\t");
//          //string a = << hex;
//          ui->plainTextEdit->appendPlainText((hex));
          std::stringstream buffer;
          buffer << dec << mev->tick;
          buffer << '\t' << deltatick;
          buffer << '\t' << mev->track;
          buffer << '\t' << hex;
          for (int i=0; i < mev->size(); i++) {
 //             ui->plainTextEdit->appendPlainText(QString::number((*mev)[i]));
             buffer << (int)(*mev)[i] << ' ';
             if ((int)(*mev)[i] < 16) buffer << ' '; // this line adds a new space in case of (int) < 16 (or (hex) < 10) to ensure equal spacing between elements of messages.
          }
          buffer << '\t'; // Places a '\t' after each line.
          ui->plainTextEdit->appendPlainText(QString::fromStdString(buffer.str()));
//          cout << endl;
          n_events = event; // number of events in file opened. Calculated after file importing.
       }
}

void MainWindow::on_actionSave_as_triggered()
{
    QString QmvfFileName = QFileDialog::getSaveFileName(
                this,
                tr("Save MusicaVisual File"),
                "//home//",
                "Musica Visual (*.mvf);;Plain Text File (*.txt);;All Files (*.*)");

    QByteArray ba = QmvfFileName.toLatin1();
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

void MainWindow::on_actionTracks_triggered()
{
    dwidtracks = new DockWidgetTracks(this);
    dwidtracks->show();
    dwidtracks->resize(300,600);
    dwidtracks->pos();
    dwidtracks->repaint();
}

void MainWindow::on_pushButton_clicked()
{
    std::stringstream stream;
    string str;
    stream << ui->plainTextEdit->toPlainText().toUtf8().constData(); // Get the entire text from the plain text input box.
    unsigned int delta, track;
    unsigned long time;
    int messg;

    notes.clear(); // Clear notes list before adding new elements.
    tempos->clear(); // Clear tempos list before adding new elements.
    pitch_max = 1;
//    pitch_min = 100;


    for (int i = 0; getline(stream,str,'\t'); i++)
    {
//        int time, delta, track;
//        int messg;
        if (i == 0)
        {
            tpq = stoi(str.substr(5));
        }

        string messg_str;
        if (i >= 5 && i%4 == 0)
        {
            time = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 1)
        {
            delta = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 2)
        {
            track = stoi(str, nullptr, 10);
        }
        if (i >= 5 && i%4 == 3)
        {
            messg = stoi(str, nullptr, 16);
            messg_str = str;
            //ui->plainTextEdit->appendPlainText(QString::fromStdString(messg_str)); // this line is only for verification
        }
        //ui->plainTextEdit->appendPlainText(QString::fromStdString(str)); // this line is only for verification
        //ui->plainTextEdit->appendPlainText(QString::number(i)); // this line is only for verification

        // Save notes in notes list:
        if (i >= 5 && i%4 == 3) // If the current element of the plain text is a midi message
        {

            if (messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) > 0) // Condition for Note on is messg_str[0] == '9' and velocity > 0
            {
                MidiNote tempnote; // Create an object MidiNote
                tempnote.t_on = time;
                tempnote.pitch = stoi(messg_str.substr(3,2), nullptr, 16); // get the pitch from the midi message string
                tempnote.track = track;
                tempnote.vel = stoi(messg_str.substr(6,2), nullptr, 16); // get velocity from the midi message string
                notes.push_back(tempnote); // Insert the tempnote into the list "notes"
                if (tempnote.pitch > pitch_max)
                {
                        pitch_max = tempnote.pitch;
                        //cout << pitch_max << endl;
                }
                if (tempnote.pitch < pitch_min)
                        pitch_min = tempnote.pitch;
                if (tempnote.track > tracks_count)
                        tracks_count = tempnote.track;
            }
            if ((messg_str[0] == '9' && stoi(messg_str.substr(6,2), nullptr, 16) == 0) || messg_str[0] == '8') // Condition for Note off is messg_str[0] == '9' and velocity == 0 or messg_str[0] == '8'
            {
                for (std::list<MidiNote>::iterator it=notes.end() ; it != notes.begin(); ) // note_off found: run the list backwards
                {
                    --it; // The pointer regress one position at the beginning, since the same message won't be on and off together.
                    if ((*it).track == track && (*it).pitch == (unsigned int)stoi(messg_str.substr(3,2), nullptr, 16)) // find the first note in the list with the same pitch and in the same track
                    {
                        (*it).t_off = time; // set its note_off time (the current time, since it has just found a note_off)
                        (*it).t_middle = (time + (*it).t_on)/2; // set its middle time ((t_on + t_off)/2)
                        break; // break this loop since the corresponding note_on to this current note_off was found!
                    }

                }
            }
            if (messg_str[0] == 'f' && messg_str[1] == 'f' && messg_str.size() >= 9) // Checks if it's a meta-message
            {
                // meta message is ill-formed.
                // meta messages must have at least three bytes:
                //    0: 0xff == meta message marker
                //    1: meta message type
                //    2: meta message data bytes to follow
                if (messg_str.substr(3,2) == "51" && messg_str.size() == 18) // Checks if it is a tempo change message
                {
                    // Meta tempo message can only be 6 bytes long.      ^ 18 is the string size: 6bytes*2hexDigits + 6 spaces (one at the end of each)
                    // The new_tempo calculated is given in microseconds per quarter note.
                    //ui->plainTextEdit->appendPlainText("MetaTempoMessage!\n"); // Line just for checking!
                    TempoChange temp_tempo_change; // Create a temporary object for tempo change
                    //temp_tempo_change.new_tempo = ((stoi(messg_str.substr(6,2), nullptr, 16) << 16) + (stoi(messg_str.substr(9,2), nullptr, 16) << 8) + stoi(messg_str.substr(12,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.new_tempo = ((stol(messg_str.substr(9,2), nullptr, 16) << 16) + (stol(messg_str.substr(12,2), nullptr, 16) << 8) + stol(messg_str.substr(15,2), nullptr, 16)); // equivalent to: return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
                    temp_tempo_change.t_on = time;
                    //cout << temp_tempo_change.new_tempo << "\n";
                    tempos->push_back(temp_tempo_change); // Stores the tempo change in the list
                }
            }

        }
    }
    total_time = time; // set the current time after processing as global variable total time from this midi processing.
    QMessageBox::information(this, tr("Processing completed"), "The midi data in the text input window was successfully completed.", QMessageBox::Ok );
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);


//    for (std::list<TempoChange>::iterator it=tempos.begin(); it != tempos.end(); ++it) // Run the  tempo change list forwards
//    {
//        ui->plainTextEdit->appendPlainText(QString::number((*it).new_tempo)); // just for checking if the tempos were stored in the list
//        ui->plainTextEdit->appendPlainText("\n");
//    }
}


void note_blocks_paint( cv::Mat image, char* window_name, int startMidiTime, int endMidiTime, int window_width, int window_height)
{
    cv::Point pt1, pt2;
    double x1, x2, y1, y2;
    //std::cout << "Paint blocks! " << pitch_min << ' ' << pitch_max << endl;
    for (std::list<MidiNote>::iterator it=notes.begin() ; it != notes.end(); ++it) // Run the list forwards
    {
        x1 = (double)window_width*((double)(*it).t_on/((double)endMidiTime - (double)startMidiTime));
        x2 = (double)window_width*((double)(*it).t_off/((double)endMidiTime - (double)startMidiTime));
        y1 = (double)window_height - (double)window_height*((double)(*it).pitch/((double)pitch_max - (double)pitch_min + 20.0));
        y2 = (double)window_height - (double)window_height*(((double)(*it).pitch + 1.0)/((double)pitch_max - (double)pitch_min + 20.0));
        pt1.x = (int)(x1); //window_width*((*it).t_on/(endMidiTime - startMidiTime));
        pt2.x = (int)(x2); //window_width*((*it).t_off/(endMidiTime - startMidiTime));
        pt1.y = (int)(y1); //window_height*((*it).pitch/(50));
        pt2.y = (int)(y2);//window_height*(((*it).pitch + 1)/(50));
        //std::cout << (*it).t_on << ' ' << (*it).t_off << ": " << (*it).pitch << '\n';
        //std::cout << pt1.x << ' ' << pt1.y << ": " << pt2.x << ' ' << pt2.y << '\n';
        cv::rectangle( image, pt1, pt2, {(*it).vel,(*it).vel,(*it).vel}, 2, 8 );
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    int window_height = ui->spinBox_2->value();
    int window_width = ui->spinBox->value();

    cv::Mat *image_win1 = new cv::Mat;
    *image_win1 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    note_blocks_paint(*image_win1, "Note blocks", 0, total_time, window_width, window_height);

    cv::namedWindow("Note blocks");
    cv::imshow("Note blocks", *image_win1);

    cv::waitKey(0);
}



// Animation

void MainWindow::on_pushButton_3_clicked()
{
    int window_height = ui->spinBox_2->value();
    int window_width = ui->spinBox->value();


    cv::Mat *image_win2 = new cv::Mat;
    *image_win2 = cv::Mat::zeros( window_height, window_width, CV_8UC3 );

    animwinP = new AnimwinP;

    cv::namedWindow("Animation");
    animbar = new AnimationBar(0, "Animation", image_win2, window_width, window_height, total_time, ui->doubleSpinBox->value(), tempos);
    animbar->show();

    animPt = new AnimPainter;
    // parei aqui!!!

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
    dwrenderprop = new DockWidRender(this);
    dwrenderprop->show();
}
