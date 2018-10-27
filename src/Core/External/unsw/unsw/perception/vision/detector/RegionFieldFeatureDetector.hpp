#ifndef PERCEPTION_VISION_DETECTOR_REGIONFIELDFEATUREDETECTOR_H_
#define PERCEPTION_VISION_DETECTOR_REGIONFIELDFEATUREDETECTOR_H_

#include "DetectorInterface.hpp"

#include "perception/vision/VisionDefinitions.hpp"

class RegionFieldFeatureDetector : public Detector
{

public:

    /*

    */
    RegionFieldFeatureDetector();

    /*
    Detects field features by making use of the regions produced by the region
    finder.
     */
    void detect(const VisionInfoIn& info_in,
                  const VisionInfoMiddle& info_middle, VisionInfoOut& info_out);

private:

    /*
    Determines the field positions of each region. The rough start and end of
    theline are calculated based on the direction, position and boundary of the
    region. No positions are calculated for regions without directions.
    */
    void determinePositions_(const VisionInfoIn& info_in,
                             const std::vector<RegionI>& regions,
                             std::vector<int8_t>& directions,
                              std::vector<std::pair<Point, Point> >& positions,
                                                    std::vector<bool>& isCurve);

    /*
    Determines the most appropriate direction for a given region based on the
    edge histogram.
    */
    void determineDirections_(VisionInfoOut& info_out,
          const std::vector<RegionI>& regions, std::vector<int8_t>& directions);

    /*
    Creates lines and circles from regions with distinct directions.
    */
    void createFeatures_(const VisionInfoIn& info_in, VisionInfoOut& info_out,
        const std::vector<RegionI>& regions, std::vector<int8_t>& directions,
        std::vector<std::pair<Point, Point> >& positions,
                                                    std::vector<bool>& isCurve);

    /*
    Interpolates between hyperparameter values based on distance.
    */
    inline int interpolateHyperparameters(const long long distance,
        const long long min, const long long max, const long long min_distance,
                                                  const long long max_distance);

    bool checkIfCurved(const RegionI& region, const Point& startPoint,
                                                         const Point& endPoint);

    // IMPLEMENTATIONS FROM THE OLD SYSTEM. TODO: Replace with nicer methods.
    float findTAngle(Point &p, RANSACLine &l);
    float findCAngle(Point &p, RANSACLine &l1, RANSACLine &l2);
    float findGradient(RANSACLine &l, Point &p);
    // for tracking the difference along the border
    // the maximum possible size
    bool border_is_white_[2 * TOP_IMAGE_COLS + 2 * TOP_IMAGE_ROWS];
};

#endif // PERCEPTION_VISION_DETECTOR_REGIONFIELDFEATUREDETECTOR_H_
