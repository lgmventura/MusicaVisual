#-------------------------------------------------
#
# Project created by QtCreator 2016-10-02T11:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicaVisualQt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    csappmidi/src-library/MidiFile.cpp \
    csappmidi/src-library/MidiEvent.cpp \
    csappmidi/src-library/MidiEventList.cpp \
    csappmidi/src-library/MidiMessage.cpp \
    csappmidi/src-library/Binasc.cpp \
    dockwidgettracks.cpp \
    dialogcolorpicker.cpp \
    midimessages.cpp \
    animationbar.cpp \
    playthread.cpp \
    dockwidrender.cpp \
    about.cpp \
    help1.cpp

HEADERS  += mainwindow.h \
    csappmidi/include/MidiFile.h \
    csappmidi/include/MidiEventList.h \
    csappmidi/include/MidiEvent.h \
    csappmidi/include/MidiMessage.h \
    csappmidi/include/Binasc.h \
    dockwidgettracks.h \
    dialogcolorpicker.h \
    midimessages.h \
    animationbar.h \
    playthread.h \
    dockwidrender.h \
    about.h \
    help1.h

FORMS    += mainwindow.ui \
    dockwidgettracks.ui \
    dialogcolorpicker.ui \
    animationbar.ui \
    dockwidrender.ui \
    about.ui \
    help1.ui


INCLUDEPATH += .
INCLUDEPATH += /home/luiz/csappmidi
INCLUDEPATH += /home/luiz/csappmidi/include
INCLUDEPATH += /home/luiz/csappmidi/src-library
INCLUDEPATH += /home/luiz/csappmidi/lib
INCLUDEPATH += /home/luiz/csappmidi/obj

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv2/core
INCLUDEPATH += /usr/local/include/opencv2/highgui
INCLUDEPATH += /usr/local/include/opencv2/imgproc
INCLUDEPATH += /usr/local/include/opencv2/flann
INCLUDEPATH += /usr/local/include/opencv2/photo
INCLUDEPATH += /usr/local/include/opencv2/video
INCLUDEPATH += /usr/local/include/opencv2/features2d
INCLUDEPATH += /usr/local/include/opencv2/objdetect
INCLUDEPATH += /usr/local/include/opencv2/calib3d
INCLUDEPATH += /usr/local/include/opencv2/ml
INCLUDEPATH += /usr/local/include/opencv2/contrib
LIBS += `pkg-config opencv --cflags --libs`

#CONFIG += static
