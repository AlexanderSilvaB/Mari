#include "FrameWriter.h"

using namespace std;
using namespace cv;

FrameWriter::FrameWriter(string fileName)
{
    this->fileName = fileName;
}

FrameWriter::FrameWriter(string fileName, int fps, Size size)
{
    this->fileName = fileName;
    writer.open(fileName, CV_FOURCC('M','J','P','G'), fps, size, true);
}

FrameWriter::~FrameWriter()
{
    if(writer.isOpened())
        writer.release();
}

void FrameWriter::write(Mat &img)
{
    if(writer.isOpened())
        writer.write(img);
    else
        imwrite(fileName, img);
}