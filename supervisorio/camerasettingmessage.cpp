#include "camerasettingmessage.h"

CameraSettingMessage::CameraSettingMessage() : Message()
{
    setType(TYPE_CAM_SETTING);
    setting = SETTING_INVALID;
    value = 0;
}

CameraSettingMessage::CameraSettingMessage(const CameraSettingMessage &message) : Message (message)
{
    setType(TYPE_CAM_SETTING);
    setting = message.setting;
    value = message.value;
}

CameraSettingMessage::CameraSettingMessage(int setting, int value) : Message ()
{
    setType(TYPE_CAM_SETTING);
    this->setting = setting;
    this->value = value;
}

int CameraSettingMessage::getSetting()
{
    return setting;
}

int CameraSettingMessage::getValue()
{
    return value;
}

QString CameraSettingMessage::toString()
{
    QString st = "invalid";
    switch (setting)
    {
        case SETTING_BRIGHTNESS:
            st = "brightness";
            break;
        case SETTING_CONTRAST:
            st = "constrast";
            break;
        case SETTING_SATURATION:
            st = "saturation";
            break;
        case SETTING_SHARPNESS:
            st = "sharpness";
            break;
        case SETTING_SAVE:
            st = "save";
            break;
        case SETTING_DISCARD:
            st = "discard";
            break;
        case SETTING_NUMBER:
            st = "number";
            break;
        default:
            break;
    }
    return QString("Camera setting [%1 = %2]").arg(st, QString::number(value));
}

int CameraSettingMessage::decode(QByteArray &data)
{
    int sz = Message::decode(data);
    QDataStream stream(data);
    stream.skipRawData(sz);

    stream >> setting;
    sz += sizeof(setting);
    stream >> value;
    sz += sizeof (value);
    return sz;
}

int CameraSettingMessage::encode(QByteArray &data)
{
    int sz = Message::encode(data);
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream.skipRawData(sz);
    stream << setting;
    stream << value;
    return data.size();
}
