#include "CameraSettingMessage.h"
#include <sstream>
#include <cstring>

using namespace std;

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

string CameraSettingMessage::toString()
{
    string st = "invalid";
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
        default:
            break;
    }
    stringstream ss;
    ss << "Camera setting [" << st << " = " << value << "]";
    return ss.str();
}

int CameraSettingMessage::decode(std::vector<char> &data)
{
    int sz = Message::decode(data);
    char *ptr = data.data();

    setting = ((int*)ptr + sz)[0];
    sz += sizeof (setting);
    value = ((int*)ptr + sz)[0];
    sz += sizeof (value);
    return sz;
}

int CameraSettingMessage::encode(std::vector<char> &data)
{
    Message::encode(data);
    char buff[8];
    int sz = 0;
    memcpy(buff + sz, (char*)(&setting), sizeof(setting));
    sz += 4;
    memcpy(buff + sz, (char*)(&value), sizeof(value));
    sz += 4;
    for(int i = 0; i < sz; i++)
        data.push_back(buff[i]);
    return data.size();
}
