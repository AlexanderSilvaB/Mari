#ifndef IMAGEMESSAGE_H
#define IMAGEMESSAGE_H

#include "message.h"

#define TYPE_IMAGE TYPE('I'+'M'+'A'+'G'+'E')

#define IMAGE_TYPE_GRAY 0
#define IMAGE_TYPE_RGB 1
#define IMAGE_TYPE_BGR 2
#define IMAGE_TYPE_HSV 3
#define IMAGE_TYPE_YUV 4

class ImageMessage : public Message
{
    private:
        QString name;
        uchar *data;
        int width, height;
        int imageType;
        int step;
        int dataSize;
    public:
        ImageMessage();
        ImageMessage(const ImageMessage &imageMessage);
        ImageMessage(QString name, int width, int height, int imageType);
        ImageMessage(QString name, int width, int height, int imageType, uchar *data);
        ~ImageMessage();

        QString getName();
        uchar *getData();
        int getWidth();
        int getHeight();
        int getImageType();
        int getStep();
        int getDataSize();

        QString toString();
        int decode(QByteArray &data);
        int encode(QByteArray &data);



};

#endif // IMAGEMESSAGE_H
