#include "videorecorder.h"

// Constructor
VideoRecorder::VideoRecorder(int size_x, int size_y, int fps, std::string outputPath, std::string codecFourCC)
{
    this->VideoDimensions = cv::Size(size_x, size_y);
    this->Fps = fps;
    this->OutputPath = outputPath;
    this->CodecFourCC = codecFourCC;
}

// Other class member functions
void VideoRecorder::setVideoDim(int size_x, int size_y)
{
    this->VideoDimensions = cv::Size(size_x, size_y);
}

void VideoRecorder::createVideoWriter()
{
    this->Video = cv::VideoWriter(this->OutputPath, cv::VideoWriter::fourcc(CodecFourCC.at(0), CodecFourCC.at(1), CodecFourCC.at(2), CodecFourCC.at(3)), Fps, VideoDimensions, true); // last true is for colour
}

void VideoRecorder::writeFrame(cv::Mat frame)
{
    if (this->RecordVideo == true)
    {
        this->Video.write(frame);
    }
}

void VideoRecorder::writeFrameBypassCheck(cv::Mat frame)
{
    this->Video.write(frame);
}

void VideoRecorder::releaseVideo()
{
    this->Video.release();
}

std::string VideoRecorder::getFileExtension()
{
    std::string fExt = CodecExtension::EXTENSION[CodecFourCC];
    return fExt;
}


std::map<std::string, std::string> CodecExtension::EXTENSION =
{
    {"X264", "mkv"},
    {"MP4V", "avi"},
    {"MJPG", "avi"}
};
