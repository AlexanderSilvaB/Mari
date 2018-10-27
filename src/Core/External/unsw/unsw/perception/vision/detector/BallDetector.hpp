#ifndef PERCEPTION_VISION_DETECTOR_BALLDETECTOR_H_
#define PERCEPTION_VISION_DETECTOR_BALLDETECTOR_H_

#include <iostream>

#include "perception/vision/detector/DetectorInterface.hpp"
#include "perception/vision/Region.hpp"
#include "types/VisionInfoOut.hpp"
#include "types/GroupLinks.hpp"

#include "types/RansacTypes.hpp"

#define MAX_RES TOP_IMAGE_COLS*TOP_IMAGE_ROWS

#define NUM_ANGLE_BINS 8
#define NUM_GREYSCALE_HISTOGRAM_BINS 16

// #define BALL_DETECTOR_USES_VDM
#ifdef BALL_DETECTOR_USES_VDM

#include "perception/vision/VisionDebuggerInterface.hpp"
#include "soccer.hpp"

#endif // BALL_DETECTOR_USES_VDM

enum PartialBallSide {
    BALL_SIDE_LEFT = 0,
    BALL_SIDE_TOP,
    BALL_SIDE_RIGHT,
    BALL_SIDE_BOTTOM,
    BALL_SIDE_TOTAL
};

struct HOGFeatures {
    int angle_counts[NUM_ANGLE_BINS];
    int opposites[4];
    int total_edge_magnitude;
    int edge_count;
    int edge_density;

    void print(){
        std::cout << "== HOG ==" << std::endl;
        std::cout << "HOG Angles: ";
        for (int i = 0; i < NUM_ANGLE_BINS; i++){ std::cout << angle_counts[i] << " "; }
        std::cout << std::endl << "Opposites: ";
        for (int i = 0; i < 4; i++){ std::cout << opposites[i] << " "; }
        std::cout << std::endl << "Total Magnitude: " << total_edge_magnitude << " Edges: " << edge_count << " Density: " << edge_density << std::endl;
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "== HOG ==" << std::endl;
        s << "HOG Angles: ";
        for (int i = 0; i < NUM_ANGLE_BINS; i++){ s << angle_counts[i] << " "; }
        s << std::endl << "Opposites: ";
        for (int i = 0; i < 4; i++){ s << opposites[i] << " "; }
        s << std::endl << "Total Magnitude: " << total_edge_magnitude << " Edges: " << edge_count << " Density: " << edge_density << std::endl;
        return s.str();
    }
};

struct PatternFeatures {
    int vertical_swaps;
    int horizontal_swaps;

    void print(){
        std::cout << "== Pattern ==" << std::endl;
        std::cout << "Vertical Swaps: " << vertical_swaps << " Horizontal Swaps: " << horizontal_swaps << std::endl;
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "== Pattern ==" << std::endl;
        s << "Vertical Swaps: " << vertical_swaps << " Horizontal Swaps: " << horizontal_swaps << std::endl;
        return s.str();
    }
};

struct InternalRegion {
    int num_pixels;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    bool completely_internal;

    void print() {
        std::cout << "- num_pixels: " << num_pixels << " completely_internal: " << completely_internal << std::endl;
        std::cout << "  p1: (" << min_x << ", " << min_y << "), p2: (" << max_x << ", " << max_y << ")" << std::endl;
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "- num_pixels: " << num_pixels << " completely_internal: " << completely_internal << std::endl;
        s << "  p1: (" << min_x << ", " << min_y << "), p2: (" << max_x << ", " << max_y << ")" << std::endl;
        return s.str();
    }
};

struct InternalRegionFeatures {
    int num_internal_regions;
    int num_regions;
    double max_internal_region_prop;
    std::vector<InternalRegion> groups;

    void print(){
        std::cout << "== Internal Regions ==" << std::endl;
        std::cout << "Num Regions: " << num_regions << " Num Internal: " << num_internal_regions << std::endl;
        for (std::vector<InternalRegion>::iterator it = groups.begin(); it != groups.end(); ++it) { it->print(); }
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "== Internal Regions ==" << std::endl;
        s << "Num Regions: " << num_regions << " Num Internal: " << num_internal_regions << std::endl;
        return s.str();
    }
};

struct SphereCheckFeatures {
    int num_darker_lower_pixels;
    int num_pixels_examined;

    void print(){
        std::cout << "== Sphere Check ==" << std::endl;
        std::cout << "Proportion Dark: " << num_darker_lower_pixels << "/" << num_pixels_examined
                  << "(" << float(num_darker_lower_pixels)/num_pixels_examined << ")" << std::endl;
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "== Sphere Check ==" << std::endl;
        s << "Proportion Dark: " << num_darker_lower_pixels << "/" << num_pixels_examined
                  << "(" << float(num_darker_lower_pixels)/num_pixels_examined << ")" << std::endl;
        return s.str();
    }
};

struct GreyscaleHistogramFeatures {
    float normalised_histogram[NUM_GREYSCALE_HISTOGRAM_BINS];

    void print(){
        std::cout << "== Greyscale Histogram ==" << std::endl;
        for (int i = 0; i < NUM_GREYSCALE_HISTOGRAM_BINS; i++) { std::cout << normalised_histogram[i] << " "; }
        std::cout << std::endl;
    }

    std::string getSummary() {
        std::ostringstream s;

        s << "== Greyscale Histogram ==" << std::endl;
        for (int i = 0; i < NUM_GREYSCALE_HISTOGRAM_BINS; i++) { s << normalised_histogram[i] << " "; }
        s << std::endl;
        return s.str();
    }
};

struct CircleFitFeatures {
    bool circle_found;
    float error;
    RANSACCircle result_circle;

    std::string getSummary() {
        std::ostringstream s;

        s << "== CircleFit ==" << std::endl;
        s << "Circle found: " << circle_found << " Centre: " << result_circle.centre.x() << "," << result_circle.centre.y()
          << " Radius: " << result_circle.radius << std::endl;;
        return s.str();
    }


#ifdef BALL_DETECTOR_USES_VDM
    void drawBall(VisionPainter *p) {
        vdm->msg << getSummary() << std::endl;
        if (circle_found) {
            p->drawCircle(
                result_circle.centre.x(), result_circle.centre.y(), result_circle.radius,
                VisionPainter::PINK
            );
        }
    }

#endif // BALL_DETECTOR_USES_VDM
};

struct BallDetectorVisionBundle {
    const RegionI* region;
    bool region_created;

    BallInfo ball;

    /* Varaibles for holding the otsu threshold.
     * Midpoint is the cutover between top and bot.
     * We split this because the ball's sphere shape makes it darker
     * at the bottom.
     */
    int otsu_bot_threshold_;
    int otsu_top_threshold_;

    double intra_class_var_top_;
    double intra_class_var_bot_;

    int otsu_midpoint_;
    // This can be an indicator of the otsu midpoint in high res.
    int original_region_base_y_;

    std::vector<double> contrast_row_multiplier;

    HOGFeatures hog;
    PatternFeatures pattern;
    CircleFitFeatures circle_fit;
    InternalRegionFeatures internal_regions;
    SphereCheckFeatures sphere_check;

    std::vector <Point> circle_fit_points;

    double diam_size_est;
    double diam_expected_size;
    double diam_expected_size_pixels;

    // Partial regions are regions that are on the edge of the frame.
    bool is_partial_region;
    bool is_crazy_ball;

    PartialBallSide partial_ball_side;

#ifdef BALL_DETECTOR_USES_VDM
    void drawBall() {
        if (vdm != NULL) {
            VisionPainter *p = vdm->getGivenRegionOverlayPainter(*region);

            if (region_created) {
                vdm->msg << "Region Created" << std::endl;
            } else {
                vdm->msg << "Region Not Created" << std::endl;
            }
            vdm->msg << std::endl;
            vdm->msg << "Diam Size Estimated: " << diam_size_est << std::endl;
            vdm->msg << "Diam Size Expected:  " << diam_expected_size;
            vdm->msg << " (" << diam_expected_size_pixels << " pixels)" << std::endl;
            vdm->msg << "Is Partial Region? " << is_partial_region << std::endl;
            vdm->msg << std::endl;
            vdm->msg << "Otsu Thresholds" << std::endl;
            vdm->msg << "Top Threshold: " << otsu_top_threshold_ << std::endl;
            vdm->msg << "Bot Threshold: " << otsu_bot_threshold_ << std::endl;
            vdm->msg << "Otsu Midpoint: " << otsu_midpoint_ << std::endl;
            vdm->msg << "Top Intra Class Var: " << intra_class_var_top_ << std::endl;
            vdm->msg << "Bot Intra Class Var: " << intra_class_var_bot_ << std::endl;
            vdm->msg << std::endl;
            vdm->msg << "Offnao: " << offNao << std::endl;

            circle_fit.drawBall(p);

            vdm->setDebugMessage();
        }
    }

#endif // BALL_DETECTOR_USES_VDM
};

class BallDetector: public Detector {
    public:

        BallDetector() : crazy_ball_cycle_(0), last_normal_ball_(0) {}

        /**
         * detect implementation of abstract infterface function
         */
        void detect(const VisionInfoIn& info_in, const VisionInfoMiddle& info_middle, VisionInfoOut& info_out);

        void processCircleFitSizeEst(const RegionI& region, BallDetectorVisionBundle &bdvb);

        /* Generate the candidate points */
        void getCircleCandidatePoints(const RegionI& region, BallDetectorVisionBundle &bdvb, bool semiCircle);

        void preProcess(BallDetectorVisionBundle &bdvb);

        std::string getFeatureSummary(VisionInfoOut& info_out, BallDetectorVisionBundle &bdvb);

        void processCircleFit(const RegionI& region, BallDetectorVisionBundle &bdvb);
        bool analyseCircleFit(CircleFitFeatures &feat);

        void regenerateRegion(BallDetectorVisionBundle &bdvb, bool aspectCheck);
        void regenerateRegionFromCircleFit(BallDetectorVisionBundle &bdvb, RANSACCircle &circle);

        void trimRegionBasedOnCircleCandidatePoints(BallDetectorVisionBundle &bdvb);

        void rescaleRegion(BallDetectorVisionBundle &bdvb);

        void getSizeEst(BallDetectorVisionBundle &bdvb, const VisionInfoIn& info_in, VisionInfoOut& info_out);

        void processInternalRegions(const RegionI &baseRegion, BallDetectorVisionBundle &bdvb,
            RANSACCircle &result_circle, InternalRegionFeatures &internal_regions);

        bool naiveROI(const VisionInfoIn& info_in, const RegionI& region, const VisionInfoMiddle& info_middle, VisionInfoOut& info_out, bool doReject, std::vector <BallDetectorVisionBundle> &res);

        bool blackROI(const VisionInfoIn& info_in, const RegionI& region, const VisionInfoMiddle& info_middle, VisionInfoOut& info_out, bool doReject, std::vector <BallDetectorVisionBundle> &res);

        bool circleROI(const VisionInfoIn& info_in, const RegionI& region, const VisionInfoMiddle& info_middle, VisionInfoOut& info_out, bool doReject, std::vector <BallDetectorVisionBundle> &res);

        bool comboROI(const VisionInfoIn& info_in, const RegionI& region, const VisionInfoMiddle& info_middle, VisionInfoOut& info_out, bool doReject, std::vector <BallDetectorVisionBundle> &res);

        // I know this is ugly copied code. But unless we template it (so the compiler knows about it and can inline)
        // Not sure how to make this clean AND efficient

        void connectedComponentAnalysisWhite(const RegionI& base_region,
            BallDetectorVisionBundle &bdvb);

        void connectedComponentAnalysisNotWhiteAndInside(const RegionI& base_region,
            BallDetectorVisionBundle &bdvb,
            RANSACCircle &circle);

        bool checkPartialRegion(BallDetectorVisionBundle &bdvb);

        void processInCircleOtsu(BallDetectorVisionBundle &bdvb);

        void findLargestCircleFit(BallDetectorVisionBundle &bdvb, float max_radius, std::vector<bool> **cons, std::vector <bool> cons_buf[2], float e, unsigned int n,
            float min_radius_prop, float step_size, PartialBallSide partial_ball_side);
        void findBestCircleFit(BallDetectorVisionBundle &bdvb, float max_radius, std::vector<bool> **cons, std::vector <bool> cons_buf[2], float e, unsigned int n,
            float min_radius_prop, float step_size, PartialBallSide partial_ball_side);
    private:
        float confidenceThatRegionIsBall(BallDetectorVisionBundle &bdvb);

        bool isHeadTiltedForward(VisionInfoOut& info_out);

        float getDiamInImage(VisionInfoOut& info_out, Point p);

        // Broken
        //void processBalls(const RegionI& base_region, BallDetectorVisionBundle &bdvb, VisionInfoOut& info_out);

        bool analyseOtsuIntraClassVar(double var);
        bool analyseOtsuThresh(int thresh);

        void processHOG(const RegionI &region, BallDetectorVisionBundle &bdvb);
        bool analyseHOG(HOGFeatures &hog_features);

        void processPattern(const RegionI &region, BallDetectorVisionBundle &bdvb);
        bool analysePattern(PatternFeatures &pattern_features);

        bool analyseInternalRegions(InternalRegionFeatures &internal_region_features, BallDetectorVisionBundle &bdvb);
        bool analyseInternalRegionsTotal(InternalRegionFeatures &internal_region_features, BallDetectorVisionBundle &bdvb);
        bool analyseInternalRegionCircles(std::vector <CircleFitFeatures> &internal_region_circles);

        void processSphereCheck(const RegionI& region, BallDetectorVisionBundle &bdvb);
        bool analyseSphereCheck(SphereCheckFeatures &sphere_check_features);

        GreyscaleHistogramFeatures processGreyscaleHistogram(const RegionI& region, BallDetectorVisionBundle &bdvb);
        bool analyseGreyscaleHistogram(GreyscaleHistogramFeatures &feat);

        bool shouldRunCrazyBallDetector(const VisionInfoIn& info_in);

        // The set of links between groups generated during CCA. Here to avoid
        // reallocation.
        GroupLinks group_links_;

        // The ID of the group for each pixel.
        uint16_t groups_[MAX_RES];

        // The number of pixels in each group.
        std::vector<int> group_counts_;

        // The smallest x value in each group.
        std::vector<int> group_low_xs_;

        // The largest x value in each group.
        std::vector<int> group_high_xs_;

        // The smallest y value in each group.
        std::vector<int> group_low_ys_;

        // The largest y value in each group.
        std::vector<int> group_high_ys_;

        // The x value of the topmost pixel
        std::vector<int> group_top_x_;

        // The y value of the topmost pixel
        std::vector<int> group_top_y_;

        int crazy_ball_cycle_;
        int last_normal_ball_;
        int last_ball_distance_;
};

#endif
