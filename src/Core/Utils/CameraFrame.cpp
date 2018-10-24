#include "CameraFrame.h"

#include <cstring>
#include <stdint.h>
#include <sys/time.h>

using namespace cv;

static int clamp(double x)
{
	if (x < 0)
		return 0;
	else if (x > 250)
		return 255;
	else
		return x;
}

#define Y_ADJ(y1) ((255 / 219.0) * (y1 - 16))
static void yuv444_to_rgb(uint8_t Y1, uint8_t Cb, uint8_t Cr, uint8_t *dst)
{
	double pb = Cb - 128;
	double pr = Cr - 128;

	double r = Y_ADJ(Y1) + 255/112.0 * 0.701 * pr;
	double g = Y_ADJ(Y1) - 255/112.0 * 0.886 * 0.114/0.587 * pb
	                     - 255/112.0 * 0.701 * 0.299/0.587 * pr;
	double b = Y_ADJ(Y1) + 255/112.0 * 0.886 * pb;

	dst[0] = clamp(b);
	dst[1] = clamp(g);
	dst[2] = clamp(r);
}

// Consumes 6 bytes of dst and 4 bytes of src.
static void yuv422_to_rgb(uint8_t const *src, uint8_t *dst)
{
	int8_t y1 = src[0];
	int8_t cb = src[1];
	int8_t y2 = src[2];
	int8_t cr = src[3];

	yuv444_to_rgb(y1, cb, cr, dst + 0);
	yuv444_to_rgb(y2, cb, cr, dst + 3);
}

CameraFrame::CameraFrame(void)
	: m_width(0),
	  m_height(0),
	  m_length(0),
	  m_data(NULL)
{
	m_time.tv_sec  = 0;
	m_time.tv_usec = 0;
}

CameraFrame::CameraFrame(CameraFrame const &src)
	: m_width(src.m_width),
	  m_height(src.m_height),
	  m_length(m_width * m_height * 3),
	  m_data(new uint8_t[m_length]),
	  m_time(src.m_time)
{
	memcpy(m_data, src.m_data, m_width * m_height);
}

CameraFrame::~CameraFrame(void)
{
	delete[] m_data;
}

CameraFrame &CameraFrame::operator=(CameraFrame const &src)
{
	Resize(src.m_width, src.m_height);
	memcpy(m_data, src.m_data, src.m_width * src.m_height * 3);
	m_time = src.m_time;
	return *this;
}

bool CameraFrame::IsValid(void) const
{
	return m_data != NULL;
}

uint32_t CameraFrame::GetWidth(void) const
{
	return m_width;
}

uint32_t CameraFrame::GetHeight(void) const
{
	return m_height;
}

uint8_t *CameraFrame::GetDataBGR(void)
{
	return m_data;
}

uint8_t const *CameraFrame::GetDataBGR(void) const
{
	return m_data;
}

timeval CameraFrame::GetTimestamp(void) const
{
	return m_time;
}

void CameraFrame::Update(uint32_t width, uint32_t height)
{
	Resize(width, height);
}

void CameraFrame::Update(uint32_t width, uint32_t height, timeval time)
{
	Resize(width, height);
	m_time = time;
}

void CameraFrame::Resize(uint32_t width, uint32_t height)
{
	if (width * height * 3 > m_length) {
		delete[] m_data;
		m_length = width * height * 3;
		m_data   = new uint8_t[m_length];
	}

	m_width  = width;
	m_height = height;
	BGR.create(m_height, m_width, CV_8UC3);
	HSV.create(m_height, m_width, CV_8UC3);
	YUV.create(m_height, m_width, CV_8UC2);
	GRAY.create(m_height, m_width, CV_8UC1);
}

void CameraFrame::ReadFromYUV422(const uint8_t *yuvData, bool rgb, bool hsv, bool gray)
{
	YUV.data = (uchar*)yuvData;
	if(rgb)
	{	
		uint32_t width  = GetWidth();
		uint32_t height = GetHeight();
		uint32_t bpl    = 640;//m_fmt_pix.fmt.pix.bytesperline;
		uint8_t *dst_data = GetDataBGR();

		for (size_t y = 0; y < height; y += 1)
		{
			for (size_t x = 0; x < width;  x += 2) 
			{
				uint8_t *src = (uint8_t *)(yuvData) + (y * bpl) + (x * 2);
				uint8_t *dst = (uint8_t *)dst_data + (y * width * 3) + (x * 3);
				yuv422_to_rgb(src, dst);
			}
		}
		BGR.data = GetDataBGR();
	}
	if(hsv && rgb)
	{
		//cv::Mat rawYuv(240, 320, CV_8UC2, yuvData);
    	//img = imdecode(rawYuv, CV_LOAD_IMAGE_COLOR);
    	//cv::cvtColor(rawYuv, img, CV_YUV2BGR_Y422);
		cvtColor(BGR, HSV, CV_BGR2HSV);
	}
	if(gray && rgb)
	{
		cvtColor(BGR, GRAY, CV_BGR2GRAY);
	}
}