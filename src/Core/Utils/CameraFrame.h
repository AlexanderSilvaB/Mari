// Thanks to biotrump/v4l2-lib
// github: biotrump/v4l2-lib

#ifndef CAMERAFRAME_H_
#define CAMERAFRAME_H_

#include <cstring>
#include <stdint.h>
#include <sys/time.h>
#include <opencv2/opencv.hpp>

class CameraFrame {
    public:
        cv::Mat BGR, HSV, YUV, GRAY;

        CameraFrame(void);
        CameraFrame(CameraFrame const &src);
        ~CameraFrame(void);

        CameraFrame &operator=(CameraFrame const &src);

        bool IsValid(void) const;
        uint32_t GetWidth(void) const;
        uint32_t GetHeight(void) const;
        timeval GetTimestamp(void) const;

        uint8_t       *GetDataBGR(void);
        uint8_t const *GetDataBGR(void) const;

        void Update(uint32_t width, uint32_t height);
        void Update(uint32_t width, uint32_t height, timeval time);

        void ReadFromYUV422(const uint8_t *yuvData, bool rgb, bool hsv, bool gray);

    private:
        uint32_t m_width;
        uint32_t m_height;
        size_t   m_length;
        uint8_t *m_data;
        timeval  m_time;

        void Resize(uint32_t width, uint32_t height);
};

#endif