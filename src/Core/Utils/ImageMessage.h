#ifndef IMAGEMESSAGE_H
#define IMAGEMESSAGE_H

#include "Message.h"
#include <cstdio>

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

        std::string toString();
        int decode(std::vector<char> &data);
        int encode(std::vector<char> &data);



};

#endif // IMAGEMESSAGE_H
