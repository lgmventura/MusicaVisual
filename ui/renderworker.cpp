#include "renderworker.h"

RenderWorker::RenderWorker(MusicData *mdt, cv::Mat *image, std::vector <cv::Mat> *img_buffer_sep_tracks, cv::Mat *playingNote, cv::Mat *movingNote, AnimWindow *aw, RenderP *rProp, std::list<LayerContainer> *layers, char* winName, AnimPainter *aPainter) { // Constructor
    this->Mdt = mdt;
    this->winName = winName;
    this->RProp = rProp;
    this->Layers = layers;
    this->Aw = aw;
    this->image = image;
    this->img_buffer_sep_tracks = img_buffer_sep_tracks;
    this->PlayingNote = playingNote;
    this->MovingNote = movingNote;
}

RenderWorker::~RenderWorker() { // Destructor
    // free resources
}

void RenderWorker::process() { // Process. Start processing data.
    // allocate resources using new here
    mutex.lock();

    //qDebug() << "Frame started";
    APainter->paintLayers(*Mdt, *image, *img_buffer_sep_tracks, *PlayingNote, *MovingNote, *Aw, *Layers, *RProp);
    //qDebug() << "Frame rendered\n";

    mutex.unlock();

    emit finished();
}
