#ifndef IMAGEMESSAGE_H
#define IMAGEMESSAGE_H

#include "Message.h"
#include <cstdio>
#include <opencv2/opencv.hpp>

#define TYPE_IMAGE TYPE('I'+'M'+'A'+'G'+'E')

#define IMAGE_TYPE_GRAY 0
#define IMAGE_TYPE_RGB 1
#define IMAGE_TYPE_BGR 2
#define IMAGE_TYPE_HSV 3
#define IMAGE_TYPE_YUV 4

class ImageMessage : public Message
{
    private:
        std::string name;
        char *data;
        int width, height;
        int imageType;
        int step;
        int dataSize;
        void update(int width, int height, int imageType);
    public:
        ImageMessage();
        ImageMessage(const ImageMessage &imageMessage);
        ImageMessage(std::string name, int width, int height, int imageType);
        ImageMessage(std::string name, int width, int height, int imageType, char *data);
        ~ImageMessage();

        std::string getName();
        char *getData();
        int getWidth();
        int getHeight();
        int getImageType();
        int getStep();
        int getDataSize();

        bool fromCV(std::string name, cv::Mat &img);

        std::string toString();
        int decode(std::vector<char> &data);
        int encode(std::vector<char> &data);



};

#endif // IMAGEMESSAGE_H
