#ifndef VISION_INFO_MIDDLE_HPP
#define VISION_INFO_MIDDLE_HPP

#include <vector>
#include "perception/vision/Region.hpp"
#include "perception/vision/colour/ColourClassifierInterface.hpp"
#include "types/CombinedFrame.hpp"

struct VisionInfoMiddle {
    std::vector<RegionI> full_regions;
    std::vector<RegionI> roi;

    ColourClassifier* colour_classifier_top_;
	ColourClassifier* colour_classifier_bot_;

    const CombinedFrame* this_frame;
};

#endif
