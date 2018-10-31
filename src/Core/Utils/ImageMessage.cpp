#include "ImageMessage.h"
#include <cstring>

using namespace std;

ImageMessage::ImageMessage() : Message ()
{
    setType(TYPE_IMAGE);
}

ImageMessage::ImageMessage(string name, int width, int height, int imageType) : Message ()
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
    data = new char[dataSize];
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
    data = new char[dataSize];
    memcpy(data, imageMessage.data, dataSize);
}

ImageMessage::ImageMessage(string name, int width, int height, int imageType, char *data) : Message()
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
    data = new char[dataSize];
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

string ImageMessage::getName()
{
    return name;
}

char *ImageMessage::getData()
{
    return data;
}

int ImageMessage::getDataSize()
{
    return dataSize;
}

string ImageMessage::toString()
{
    return "Image message: "+name;
}

int ImageMessage::decode(std::vector<char> &data)
{
    int sz = Message::decode(data);
    char *ptr = data.data();

    int nameLen;
    nameLen = ((int*)ptr + sz)[0];
    sz += sizeof (nameLen);
    char *str = new char[nameLen+1];
    memcpy(str, ptr + sz, nameLen);
    str[nameLen] = '\0';
    sz += nameLen;
    name = string(str);

    width = ((int*)ptr + sz)[0];
    sz += sizeof (width);
    height = ((int*)ptr + sz)[0];
    sz += sizeof (height);
    imageType = ((int*)ptr + sz)[0];
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
    this->data = new char[dataSize];
    memcpy(this->data, ptr + sz, dataSize);
    sz += dataSize;
    return sz;
}

int ImageMessage::encode(std::vector<char> &data)
{
    Message::encode(data);
    
    char buff[name.size() + 128];
    int sz = 0;

    int len = name.length();
    memcpy(buff + sz, (char*)(&len), sizeof(len));
    sz += 4;
    memcpy(buff + sz, name.c_str(), len);

    memcpy(buff + sz, (char*)(&width), sizeof(width));
    sz += 4;
    memcpy(buff + sz, (char*)(&height), sizeof(height));
    sz += 4;
    memcpy(buff + sz, (char*)(&imageType), sizeof(imageType));
    sz += 4;
    for(int i = 0; i < sz; i++)
        data.push_back(buff[i]);
    sz = data.size();
    data.resize(sz + dataSize);
    char *ptr = data.data() + sz;
    for(int i = 0; i < dataSize; i++)
        ptr[i] = this->data[i];
    return data.size();
}
