#ifndef _FRAME_WRITER_H_
#define _FRAME_WRITER_H_

#include <string>
#include <opencv2/opencv.hpp>

class FrameWriter
{
    private:
        cv::VideoWriter writer;
        std::string fileName;        
    public:
        FrameWriter(std::string fileName);
        FrameWriter(std::string fileName, int fps, cv::Size size);
        ~FrameWriter();

        void write(cv::Mat &img);
};

#endif
