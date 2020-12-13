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
    chords.cpp \
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
    chords.h \
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
    help1.h \
    csappmidi/include/Binasc.h \
    csappmidi/include/MidiEvent.h \
    csappmidi/include/MidiEventList.h \
    csappmidi/include/MidiFile.h \
    csappmidi/include/MidiMessage.h \
    csappmidi/include/Options.h

FORMS    += mainwindow.ui \
    dockwidgettracks.ui \
    dialogcolorpicker.ui \
    animationbar.ui \
    dockwidrender.ui \
    about.ui \
    help1.ui


INCLUDEPATH += .
INCLUDEPATH += csappmidi
INCLUDEPATH += csappmidi/include
INCLUDEPATH += csappmidi/src-library
INCLUDEPATH += csappmidi/lib
INCLUDEPATH += csappmidi/obj

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


INCLUDEPATH += /usr/include/python3.8
#INCLUDEPATH += /media/luiz/Volume/anaconda3/envs/dev/include/python3.8
LIBS += `pkg-config opencv --cflags --libs`

#LIBS += -L/usr/lib/x86_64-linux-gnu
#LIBS += -Wl,-Bstatic -lpython3.8 -Wl,-Bdynamic
#LIBS += -lz -lexpat -ldl -lutil
#LIBS += -I/usr/include/python3.8 -I/usr/include/python3.8  -Wno-unused-result -Wsign-compare -g -fdebug-prefix-map=/build/python3.8-xhc1jt/python3.8-3.8.5=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O3 -Wall

#QMAKE_CFLAGS += `-I/usr/include/python3.8 -I/usr/include/python3.8  -Wno-unused-result -Wsign-compare -g -fdebug-prefix-map=/build/python3.8-xhc1jt/python3.8-3.8.5=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O3 -Wall`
#QMAKE_LFLAGS += `-L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu -L/usr/lib  -lcrypt -lpthread -ldl  -lutil -lm -lm`

#CONFIG += static
