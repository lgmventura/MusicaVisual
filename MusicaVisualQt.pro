#-------------------------------------------------
#
# Project created by QtCreator 2016-10-02T11:11:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicaVisualQt
TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp\
    math/hexagonlib.cpp \
    musiclib/chords.cpp \
    musiclib/importmidi.cpp \
    musiclib/midimessages.cpp \
    musiclib/musicdata.cpp \
    ui/about.cpp \
    ui/animationbar.cpp \
    ui/blocklayersetup.cpp \
    ui/colourwidget.cpp \
    ui/help.cpp \
    ui/layersetup.cpp \
    ui/mainwindow.cpp \
    ui/playthread.cpp \
    ui/rendersetup.cpp \
    ui/trackssetup.cpp \
    ui/chordlayersetup.cpp \
    utils/cnum.cpp \
    csappmidi/src-library/MidiFile.cpp \
    csappmidi/src-library/MidiEvent.cpp \
    csappmidi/src-library/MidiEventList.cpp \
    csappmidi/src-library/MidiMessage.cpp \
    csappmidi/src-library/Binasc.cpp \
    utils/colour.cpp \
    utils/files.cpp \
    visual/animation.cpp \
    visual/blocklayers.cpp \
    visual/chordlayers.cpp \
    visual/eulertonnetz.cpp \
    visual/layer.cpp \
    visual/renderChordStar.cpp \
    visual/renderbuffer.cpp \
    visual/tonnetzrenderer.cpp \
    visual/videorecorder.cpp

HEADERS  += \
    math/hexagonlib.h \
    musiclib/chords.h \
    musiclib/importmidi.h \
    musiclib/midimessages.h \
    musiclib/musicdata.h \
    ui/about.h \
    ui/animationbar.h \
    ui/blocklayersetup.h \
    ui/colourwidget.h \
    ui/help.h \
    ui/layersetup.h \
    ui/mainwindow.h \
    ui/playthread.h \
    ui/rendersetup.h \
    ui/trackssetup.h \
    ui/chordlayersetup.h \
    utils/cnum.h \
    csappmidi/include/MidiFile.h \
    csappmidi/include/MidiEventList.h \
    csappmidi/include/MidiEvent.h \
    csappmidi/include/MidiMessage.h \
    csappmidi/include/Binasc.h \
    csappmidi/include/Binasc.h \
    csappmidi/include/MidiEvent.h \
    csappmidi/include/MidiEventList.h \
    csappmidi/include/MidiFile.h \
    csappmidi/include/MidiMessage.h \
    csappmidi/include/Options.h \
    utils/colour.h \
    utils/files.h \
    visual/animation.h \
    visual/blocklayers.h \
    visual/chordlayers.h \
    visual/eulertonnetz.h \
    visual/layer.h \
    visual/renderChordStar.h \
    visual/renderbuffer.h \
    visual/tonnetzrenderer.h \
    visual/videorecorder.h

FORMS    += \
    ui/about.ui \
    ui/animationbar.ui \
    ui/blocklayersetup.ui \
    ui/help.ui \
    ui/layersetup.ui \
    ui/mainwindow.ui \
    ui/rendersetup.ui \
    ui/trackssetup.ui \
    ui/chordlayersetup.ui


INCLUDEPATH += .
INCLUDEPATH += csappmidi
INCLUDEPATH += csappmidi/include
INCLUDEPATH += csappmidi/src-library
INCLUDEPATH += csappmidi/lib
INCLUDEPATH += csappmidi/obj
INCLUDEPATH += musiclib/
INCLUDEPATH += ui/

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

RESOURCES += \
    resourcelist.qrc
