#ifndef FEATURE_EXTRACTOR_H_
#define FEATURE_EXTRACTOR_H_

#include "Core/Module.h"
#include "Core/Utils/Circle.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "Core/Utils/CameraFrame.h"

#include "Core/SpellBook.h"

#include "Core/External/naoleague/featureextraction/img_processing.h"
#include "Core/External/naoleague/featureextraction/line_detection.h"
#include "Core/External/naoleague/featureextraction/line_feature_detection.h"
#include "Core/External/naoleague/featureextraction/feature_extraction.h"
#include "Core/External/naoleague/featureextraction/dis_ang_translation.h"
#include <opencv2/core/core.hpp>

#define L 0
#define T 1
#define X 2
#define UNKNOWN 4

using namespace std;

class FeatureExtractor : public InnerModule
{
private:
    float targetYaw, targetPitch;
    float distance;
    float speed;

    cv::Mat img_rgb;
    std::vector<field_point> result_intersections, actual_features; //features
    std::vector<goalposts> goalPosts;

public:
    FeatureExtractor(SpellBook *spellBook);
    void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
};

#endif
