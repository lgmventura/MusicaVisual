#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include "opencv2/highgui.hpp"
#include <map>

class VideoRecorder
{
public:
    VideoRecorder(int size_x, int size_y, int fps = 30, std::string outputPath = ".", std::string codecFourCC = "X264");

    bool            RecordVideo = false;

    int             Fps;
    std::string     OutputPath = ".";
    std::string     CodecFourCC = std::string("X264");

private:
    cv::Size        VideoDimensions;
    cv::VideoWriter Video;
    bool            Ready;
public:
    void setVideoDim(int size_x, int size_y);
    void createVideoWriter();
    void writeFrame(cv::Mat frame);
    void releaseVideo();
    std::string getFileExtension();
};

class CodecExtension
{
private:
    CodecExtension() {}
public:
    static std::map <std::string, std::string> EXTENSION;
};

#endif // VIDEORECORDER_H
