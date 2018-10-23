#ifndef _RELATIVE_COORDS_H_
#define _RELATIVE_COORDS_H_

class RelativeCoords
{
    private:
        float distance, yaw, pitch;
    public:
        RelativeCoords();
        RelativeCoords(const RelativeCoords &coord);
        RelativeCoords(float distance, float yaw, float pitch);

        float getDistance();
        float getYaw();
        float getPitch();

        void setDistance(float distance);
        void setYaw(float yaw);
        void setPitch(float pitch);

        void fromPixel(int w, int h, float headYaw = 0.0f, float headPitch = 0.0f);   

};

#endif
