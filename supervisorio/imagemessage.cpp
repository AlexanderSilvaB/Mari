#include "imagemessage.h"
#include <QDataStream>

ImageMessage::ImageMessage() : Message ()
{
    setType(TYPE_IMAGE);
}

ImageMessage::ImageMessage(QString name, int width, int height, int imageType) : Message ()
{
    setType(TYPE_IMAGE);
    this->name = name;
    this->width = width;
    this->height = height;
    this->imageType = imageType;
    dataSize = width*height;
    step = width;
    switch (imageType)
    {
        case IMAGE_TYPE_GRAY:
            break;
        case IMAGE_TYPE_BGR:
        case IMAGE_TYPE_RGB:
        case IMAGE_TYPE_HSV:
            dataSize *= 3;
            step *= 3;
            break;
        case IMAGE_TYPE_YUV:
            dataSize *= 2;
            step *= 2;
            break;
    }
    data = new uchar[dataSize];
}

ImageMessage::ImageMessage(const ImageMessage &imageMessage) : Message (imageMessage)
{
    setType(TYPE_IMAGE);
    name = imageMessage.name;
    width = imageMessage.width;
    height = imageMessage.height;
    imageType = imageMessage.imageType;
    dataSize = imageMessage.dataSize;
    step = imageMessage.step;
    data = new uchar[dataSize];
    memcpy(data, imageMessage.data, dataSize);
}

ImageMessage::ImageMessage(QString name, int width, int height, int imageType, uchar *data) : ImageMessage(name, width, height, imageType)
{
    memcpy(this->data, data, dataSize);
}

ImageMessage::~ImageMessage()
{
    delete[] data;
}

int ImageMessage::getWidth()
{
    return width;
}

int ImageMessage::getHeight()
{
    return height;
}

int ImageMessage::getImageType()
{
    return imageType;
}

int ImageMessage::getStep()
{
    return step;
}

QString ImageMessage::getName()
{
    return name;
}

uint8_t *ImageMessage::getData()
{
    return data;
}

int ImageMessage::getDataSize()
{
    return dataSize;
}

QString ImageMessage::toString()
{
    return "Image message: "+name;
}

int ImageMessage::decode(QByteArray &data)
{
    int sz = Message::decode(data);
    QDataStream stream(data);
    stream.skipRawData(sz);

    int nameLen;
    stream >> nameLen;
    sz += sizeof(nameLen);
    char *str = new char[nameLen+1];
    stream.readRawData(str, nameLen);
    sz += nameLen;
    str[nameLen] = '\0';
    name = QString(str);

    stream >> width;
    sz += sizeof (width);
    stream >> height;
    sz += sizeof (height);
    stream >> imageType;
    sz += sizeof (imageType);
    dataSize = width*height;
    step = width;
    switch (imageType)
    {
        case IMAGE_TYPE_GRAY:
            break;
        case IMAGE_TYPE_BGR:
        case IMAGE_TYPE_RGB:
        case IMAGE_TYPE_HSV:
            dataSize *= 3;
            step *= 3;
            break;
        case IMAGE_TYPE_YUV:
            dataSize *= 2;
            step *= 2;
            break;
    }
    this->data = new uchar[dataSize];
    stream.readRawData((char*)this->data, dataSize);
    sz += dataSize;
    return sz;
}

int ImageMessage::encode(QByteArray &data)
{
    int sz = Message::encode(data);
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream.skipRawData(sz);
    stream << name.length();
    stream.writeRawData(name.toStdString().c_str(), name.length());
    stream << width;
    stream << height;
    stream << imageType;
    stream.writeRawData((char*)this->data, dataSize);
    return data.size();
}
