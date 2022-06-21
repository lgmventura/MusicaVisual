#ifndef RENDERWORKER_H
#define RENDERWORKER_H

#include <QObject>
#include <QMutex>

#include "opencv2/core.hpp"

#include "musiclib/musicdata.h"
#include "midimessages.h"
#include "rendersetup.h"

class RenderWorker : public QObject {
    Q_OBJECT
public:
    RenderWorker(MusicData *mdt, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, cv::Mat *playingNote, cv::Mat *movingNote, AnimWindow *aw, RenderP *rProp, std::list<LayerContainer> *layers, char* winName, AnimPainter *aPainter);
    ~RenderWorker();
public slots:
    void process();
signals:
    void finished();
    void error(QString err);
private:
    MusicData *Mdt;
    char* winName;
    cv::Mat *image;
    std::vector <cv::Mat> *img_buffer_sep_tracks;
    cv::Mat *PlayingNote;
    cv::Mat *MovingNote;
    AnimWindow *Aw;
    RenderP *RProp;
    std::list<LayerContainer> *Layers;
    AnimPainter *APainter;

    QMutex mutex;
};

#endif // RENDERWORKER_H
