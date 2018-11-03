#ifndef CAMERASETTINGMESSAGE_H
#define CAMERASETTINGMESSAGE_H

#include "message.h"

#define TYPE_CAM_SETTING TYPE('C'+'A'+'M'+'S'+'E'+'T')

#define SETTING_INVALID -1
#define SETTING_BRIGHTNESS 0
#define SETTING_SATURATION 1
#define SETTING_CONTRAST 2
#define SETTING_SHARPNESS 3
#define SETTING_SAVE 98
#define SETTING_DISCARD 99
#define SETTING_NUMBER 50


class CameraSettingMessage : public Message
{
    private:
        int setting;
        int value;
    public:
        CameraSettingMessage();
        CameraSettingMessage(const CameraSettingMessage &message);
        CameraSettingMessage(int setting, int value);

        int getSetting();
        int getValue();

        QString toString();
        int decode(QByteArray &data);
        int encode(QByteArray &data);

};

#endif // CAMERASETTINGMESSAGE_H
