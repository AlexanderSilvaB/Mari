#include "perception/vision/detector/RegionFieldFeatureDetector.hpp"
#include "perception/vision/VisionDefinitions.hpp"

#include "types/FieldFeatureInfo.hpp"
#include "types/RansacTypes.hpp"
#include "types/Point.hpp"

#include "utils/SPLDefs.hpp"

// The magnitude of an edge at base resolution for it to be considered
// significant.
#define STRONG_EDGE_THRESHOLD_BASE 20
#define STRONG_EDGE_THRESHOLD_TOP \
        ((STRONG_EDGE_THRESHOLD_BASE*TOP_SALIENCY_DENSITY) * \
                              (STRONG_EDGE_THRESHOLD_BASE*TOP_SALIENCY_DENSITY))
#define STRONG_EDGE_THRESHOLD_BOT \
        ((STRONG_EDGE_THRESHOLD_BASE*BOT_SALIENCY_DENSITY*2) * \
                            (STRONG_EDGE_THRESHOLD_BASE*BOT_SALIENCY_DENSITY*2))

// How many buckets should be used in the HOG calculations.
#define HOG_BUCKETS 8

// The maximum number of features localisation can handle.
#define MAX_LOCALISATION_FEATURES 7

// How wide an area a region may be in to be added to a line.
#define MIN_LINE_CONNECTION_DISTANCE_CLOSE (100)
#define MIN_LINE_CONNECTION_DISTANCE_FAR (200)
#define MIN_LINE_CONNECTION_DISTANCE_DISTANCE_CLOSE (500)
#define MIN_LINE_CONNECTION_DISTANCE_DISTANCE_FAR (5000)
#define MIN_LINE_CONNECTION_DISTANCE_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, MIN_LINE_CONNECTION_DISTANCE_CLOSE, \
    MIN_LINE_CONNECTION_DISTANCE_FAR, \
    MIN_LINE_CONNECTION_DISTANCE_DISTANCE_CLOSE, \
                                      MIN_LINE_CONNECTION_DISTANCE_DISTANCE_FAR)

// Maximum distance between a new and old line end point allowed.
#define MAX_LINE_EXTENSION_DISTANCE_CLOSE (1000*1000)
#define MAX_LINE_EXTENSION_DISTANCE_FAR (2000*2000)
#define MAX_LINE_EXTENSION_DISTANCE_DISTANCE_CLOSE (1000*1000)
#define MAX_LINE_EXTENSION_DISTANCE_DISTANCE_FAR (4000*4000)
#define MAX_LINE_EXTENSION_DISTANCE_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, MAX_LINE_EXTENSION_DISTANCE_CLOSE, \
    MAX_LINE_EXTENSION_DISTANCE_FAR, \
    MAX_LINE_EXTENSION_DISTANCE_DISTANCE_CLOSE, \
                                       MAX_LINE_EXTENSION_DISTANCE_DISTANCE_FAR)

// The minimum allowable line length.
#define MINIMUM_LINE_LENGTH_CLOSE (200*200)
#define MINIMUM_LINE_LENGTH_FAR (200*200)
#define MINIMUM_LINE_LENGTH_CLOSE_DISTANCE (2000*2000)
#define MINIMUM_LINE_LENGTH_FAR_DISTANCE (3000*2000)
#define MINIMUM_LINE_LENGTH_INTERPOLATE(distance) interpolateHyperparameters( \
    distance, MINIMUM_LINE_LENGTH_CLOSE, MINIMUM_LINE_LENGTH_FAR, \
           MINIMUM_LINE_LENGTH_CLOSE_DISTANCE, MINIMUM_LINE_LENGTH_FAR_DISTANCE)

// The numerator and denominator of the fraction of edges that must be in a
// particular direction for a line to be considered to be in that direction.
#define STRONG_DIRECTION_FRACTION_NUMERATOR 3
#define STRONG_DIRECTION_FRACTION_DENOMINATOR 4

// The numerator and denominator of the ratio of edges between two buckets for
// the strong line direction to be valid.
#define STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR 0
#define STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR 100

// The maximum error allowed for lines to be considered perpendicular.
#define MAX_PERPENDICULAR_ANGLE_ERROR (M_PI/8)

// The excess distance that must be allowed on eigher side of a T intersection.
#define T_INTERSECTION_EXCESS (100*100)

// The maximum error allowed in goal box detection.
#define MAX_GOAL_BOX_ERROR (200)

// The minimum line length required to form a corner or T.
#define MIN_FEATURE_LINE_LENGTH_CLOSE (1000*1000)
#define MIN_FEATURE_LINE_LENGTH_FAR (1500*1500)
#define MIN_FEATURE_LINE_LENGTH_CLOSE_DISTANCE (1000*1000)
#define MIN_FEATURE_LINE_LENGTH_FAR_DISTANCE (3000*3000)
#define MIN_FEATURE_LINE_LENGTH_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, MIN_FEATURE_LINE_LENGTH_CLOSE, \
    MIN_FEATURE_LINE_LENGTH_FAR, MIN_FEATURE_LINE_LENGTH_CLOSE_DISTANCE, \
                                           MIN_FEATURE_LINE_LENGTH_FAR_DISTANCE)

// The minimum line length required for a goal box corner.
#define MIN_FEATURE_LINE_LENGTH_GOAL_BOX (700*700)

// How far an intersection point can be from the nearest line end.
#define MAX_LINE_END_INTERSECTION_DISTANCE_CLOSE (200*200)
#define MAX_LINE_END_INTERSECTION_DISTANCE_FAR (600*600)
#define MAX_LINE_END_INTERSECTION_DISTANCE_CLOSE_DISTANCE (1000*1000)
#define MAX_LINE_END_INTERSECTION_DISTANCE_FAR_DISTANCE (3000*3000)
#define MAX_LINE_END_INTERSECTION_DISTANCE_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, \
    MAX_LINE_END_INTERSECTION_DISTANCE_CLOSE, \
    MAX_LINE_END_INTERSECTION_DISTANCE_FAR, \
    MAX_LINE_END_INTERSECTION_DISTANCE_CLOSE_DISTANCE, \
                                MAX_LINE_END_INTERSECTION_DISTANCE_FAR_DISTANCE)

// The multiplier by which regions should be expanded to make sure edges are
// included.
#define REGION_EXPANSION_MULTIPLIER 1.1f

// The portion of a centre circle that must be seen for one to be generated.
#define CENTRE_CIRCLE_PORTION_CLOSE 0.3f
#define CENTRE_CIRCLE_PORTION_FAR 0.4f

// The total line length to reach the desired centre circle portion.
#define MIN_CENTRE_CIRCLE_LENGTH_CLOSE ((int)(CENTRE_CIRCLE_PORTION_CLOSE * \
                                          ((float)CENTER_CIRCLE_DIAMETER*M_PI)))
#define MIN_CENTRE_CIRCLE_LENGTH_FAR ((int)(CENTRE_CIRCLE_PORTION_FAR * \
                                          ((float)CENTER_CIRCLE_DIAMETER*M_PI)))
#define MIN_CENTRE_CIRCLE_LENGTH_CLOSE_DISTANCE (500*500)
#define MIN_CENTRE_CIRCLE_LENGTH_FAR_DISTANCE (900*900)
#define MIN_CENTRE_CIRCLE_LENGTH_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, MIN_CENTRE_CIRCLE_LENGTH_CLOSE, \
    MIN_CENTRE_CIRCLE_LENGTH_FAR, MIN_CENTRE_CIRCLE_LENGTH_CLOSE_DISTANCE, \
                                          MIN_CENTRE_CIRCLE_LENGTH_FAR_DISTANCE)

// The error in mm allowed for centre circle line ends.
#define CENTRE_CIRCLE_ERROR_CLOSE 300
#define CENTRE_CIRCLE_ERROR_FAR 300
#define CENTRE_CIRCLE_ERROR_CLOSE_DISTANCE (500*500)
#define CENTRE_CIRCLE_ERROR_FAR_DISTANCE (700*700)
#define CENTRE_CIRCLE_ERROR_INTERPOLATE(distance) \
    interpolateHyperparameters(distance, CENTRE_CIRCLE_ERROR_CLOSE, \
    CENTRE_CIRCLE_ERROR_FAR, CENTRE_CIRCLE_ERROR_CLOSE_DISTANCE, \
                                               CENTRE_CIRCLE_ERROR_FAR_DISTANCE)

// How close a line can pass to the centre of the centre circle before it is
// culled.
#define MIN_CENTRE_CIRCLE_DISTANCE_FROM_CENTRE 600

// How close a line must pass to the centre of a circle to be considered a valid
// centre line.
#define MAX_CENTRE_CIRCLE_LINE_DISTANCE_FROM_CIRCLE_CENTRE 300

// The minimum acceptable length of a centre circle centre line.
#define MIN_CENTRE_CIRCLE_LINE_LENGTH (200*200)

// The minimum distance a line must be from the centre circle.
#define MIN_DISTANCE_TO_CENTRE_CIRCLE (2000*2000)

// The maximum distance a centre circle line may be from a centre circle.
#define MAX_CENTRE_CIRCLE_LINE_END_DISTANCE (500*500)

#define DISTANCE_SQR(a, b) (pow(a.x() - b.x(), 2) + pow(a.y() - b.y(), 2))
#define DISTANCE(a, b) sqrt(DISTANCE_SQR(a, b))

//#define RFFD_USES_VATNAO
#ifdef RFFD_USES_VATNAO
#include "soccer.hpp"
#include "../VisionDebuggerInterface.hpp"
#endif // RFFD_USES_VATNAO

RegionFieldFeatureDetector::RegionFieldFeatureDetector() {
#ifdef RFFD_USES_VATNAO
    if (vdm != NULL) {
        vdm->addOption("Show Points");
        vdm->addOption("Show Border");
        vdm->addOption("Show Saliency");
    }
#endif
}

/*
Detects field features by making use of the regions produced by the region
finder.
 */
void RegionFieldFeatureDetector::detect(const VisionInfoIn& info_in,
                   const VisionInfoMiddle& info_middle, VisionInfoOut& info_out)
{
    // The regions to be examined.
    const std::vector<RegionI>& regions = info_middle.roi;

    // Clear existing field features. TODO: Test if this is needed.
    info_out.features.clear();

    // The direction associated with each region. -1 is no direction, 0 through
    // 3 is up down, upper right lower left, right left and lower right upper
    // left respectively.
    std::vector<int8_t> directions(regions.size());

    // Whether each region is a curve.
    std::vector<bool> isCurve(regions.size(), false);

    // The field positions of the regions.
    std::vector<std::pair<Point, Point> > field_positions(regions.size());

    // Determine the direction of each region.
    determineDirections_(info_out, regions, directions);

    // Determine the position of each region.
    determinePositions_(info_in, regions, directions, field_positions, isCurve);

    // Combine the directed regions into lines, goal boxes and circles.
    createFeatures_(info_in, info_out, regions, directions, field_positions,
                                                                       isCurve);

    // Finally combine the lines and circles into composite features.
    //createCompositeFeatures_(field_features);

#ifdef RFFD_USES_VATNAO
    if (vdm != NULL)
    {
        vdm->setDebugMessage();
    }
#endif // RFFD_USES_VATNAO
}

/*
Determines the most appropriate direction for a given region based on the edge
histogram.
*/
void RegionFieldFeatureDetector::determineDirections_(VisionInfoOut& info_out,
          const std::vector<RegionI>& regions, std::vector<int8_t>& directions)
{
    // Run through all the regions.
    for(unsigned int regionID=0; regionID<regions.size(); ++regionID)
    {
        // The region currently being analysed.
        const RegionI region(regions[regionID],
            regions[regionID].getBoundingBoxRel().expand(
                                                  REGION_EXPANSION_MULTIPLIER));

        // The threshold at which an edge is considered significant.
        const int edge_threshold = region.isTopCamera() ?
                          STRONG_EDGE_THRESHOLD_TOP : STRONG_EDGE_THRESHOLD_BOT;

        // The number of significant edges found in each direction.
        int hog_buckets[HOG_BUCKETS];

        // The total number of significant edges in the region.
        int significantEdges = 0;

        // The total of several HOG buckets combined.
        float combined_buckets1, combined_buckets2;

        // Zero initialise the HOG buckets.
        for(int bucket=0; bucket<HOG_BUCKETS; ++bucket)
            hog_buckets[bucket] = 0;

        // Run through the pixels within the region.
        const RegionI::iterator_fovea end = region.end_fovea();
        for(RegionI::iterator_fovea it = region.begin_fovea(); it < end; ++it)
        {
            // Check that the region is below the field boundary.
            if(info_out.topStartScanCoords[region.getBoundingBoxRaw().a.x()] <
                    region.getBoundingBoxRaw().a.y() +
                                         (!region.isTopCamera())*TOP_IMAGE_ROWS)
            {
                // The directional edge magnitudes of the pixel.
                const Point edge = it.edge();

                // Minor edges are just noise and should be ignored.
                if (edge.squaredNorm() < edge_threshold)
                    continue;

                // A new significant edge has been found.
                ++significantEdges;

                // Point analysis to figure out the bin that this point belongs
                // to.
                int quadrant = 0;
                if (edge.x() > 0){
                    quadrant = (edge.y() > 0) ? 0 : 1;
                } else {
                    quadrant = (edge.y() > 0) ? 3 : 2;
                }

                // Break quadrant up. Note that we need to use abs() for some of
                // the vectors because x, y may be negative in some quadrants.
                switch(quadrant)
                {
                    // Both positive.
                    case 0:
                        if(edge.x() < edge.y())
                            ++hog_buckets[0];
                        else
                            ++hog_buckets[1];
                        break;

                    // Y negative.
                    case 1:
                        if(edge.x() > -edge.y())
                            ++hog_buckets[2];
                        else
                            ++hog_buckets[3];
                        break;

                    // Both negative.
                    case 2:
                        if(edge.x() > edge.y())
                            ++hog_buckets[4];
                        else
                            ++hog_buckets[5];
                        break;

                    // X negative.
                    case 3:
                        if(-edge.x() > edge.y())
                            ++hog_buckets[6];
                        else
                            ++hog_buckets[7];
                        break;
                }
            }
        }

        // Check if there is a distinct direction to the region.
        directions[regionID] = -1;

        // Top to bottom direction.
        combined_buckets1 = hog_buckets[7]+hog_buckets[0];
        combined_buckets2 = hog_buckets[3]+hog_buckets[4];

        // To be a valid direction the buckets must contain 2/3rds of the
        // edges and not be too imbalanced.
        if((combined_buckets1+combined_buckets2) *
                STRONG_DIRECTION_FRACTION_DENOMINATOR >
                significantEdges*STRONG_DIRECTION_FRACTION_NUMERATOR &&
                combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR
                && combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR)
            directions[regionID] = 2;

        // Upper right to bottom left direction.
        combined_buckets1 = hog_buckets[0]+hog_buckets[1];
        combined_buckets2 = hog_buckets[4]+hog_buckets[5];
        if((combined_buckets1+combined_buckets2) *
                STRONG_DIRECTION_FRACTION_DENOMINATOR >
                significantEdges*STRONG_DIRECTION_FRACTION_NUMERATOR &&
                combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR
                && combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR)
            directions[regionID] = 1;

        // Right to left direction.
        combined_buckets1 = hog_buckets[1]+hog_buckets[2];
        combined_buckets2 = hog_buckets[5]+hog_buckets[6];
        if((combined_buckets1+combined_buckets2) *
                STRONG_DIRECTION_FRACTION_DENOMINATOR >
                significantEdges*STRONG_DIRECTION_FRACTION_NUMERATOR &&
                combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR
                && combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR)
            directions[regionID] = 0;

        // Bottom right to top left direction.
        combined_buckets1 = hog_buckets[2]+hog_buckets[3];
        combined_buckets2 = hog_buckets[6]+hog_buckets[7];
        if((combined_buckets1+combined_buckets2) *
                STRONG_DIRECTION_FRACTION_DENOMINATOR >
                significantEdges*STRONG_DIRECTION_FRACTION_NUMERATOR &&
                combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR
                && combined_buckets2*STRONG_DIRECTION_RATIO_FRACTION_NUMERATOR
                < combined_buckets1*STRONG_DIRECTION_RATIO_FRACTION_DENOMINATOR)
            directions[regionID] = 3;
    }
}

enum BorderTraverseStates {
    X_INCREMENT,
    Y_INCREMENT,
    X_DECREMENT,
    Y_DECREMENT
};

std::pair<Point, BorderTraverseStates> nextBorderPoint(int x, int y, BorderTraverseStates state, int cols, int rows) {
    if (state == X_INCREMENT) {
        if (x < cols - 1) {
            x++;
        } else {
            state = Y_INCREMENT;
        }
    }
    if (state == Y_INCREMENT) {
        if (y < rows - 1) {
            y++;
        } else {
            state = X_DECREMENT;
        }
    }
    if (state == X_DECREMENT) {
        if (x > 0) {
            x--;
        } else {
            state = Y_DECREMENT;
        }
    }
    if (state == Y_DECREMENT) {
        y--;
    }
    return std::pair<Point, BorderTraverseStates>(Point(x, y), state);
}

#define WHITE true
// The number of blacks allowed before switching from white
#define SWITCH_RATE 1
// Minimum allowed size for a stretch of white to be considered a point
#define MIN_SIZE 3
// The maximum difference between the longest and second longest points
#define PERMITTED_DIFF 0.3

bool still_white(bool *border, int i, bool was_white, int num_whites, int num_blacks) {
    if (was_white) {
        if (border[i] == WHITE || num_blacks < SWITCH_RATE) {
            return true;
        }
    } else {
        if (border[i] == WHITE) {
            return true;
        }
    }
    return false;
}

int findFirstBlack(bool *border, int length) {
    int num_blacks = 0;
    for (int i = 0; i < length; i++) {
        if (border[i] == WHITE) {
            num_blacks = 0;
        } else {
            num_blacks++;
        }

        if (num_blacks == SWITCH_RATE + 1) {
            // If we've found enough blacks to break a white streak
            return i;
        }
    }

    // If we haven't found enough blacks to break white
    return -1;
}

void findPointsFromBorder(bool *border, int length, std::vector<int> &border_points) {
    int longest_white = -1;
    int longest_white_length = 0;
    int second_longest_white = -1;
    int second_longest_white_length = 0;

    // find the first point where we have a definite black
    int i = findFirstBlack(border, length);
    if (i == -1) {
        return;
    }

    bool was_white = false;
    int num_whites = 0;
    int num_blacks = 1;
    int start_point = i;
    bool passed_zero = false;

    #ifdef RFFD_USES_VATNAO
    if (vdm != NULL && vdm->vision_debug_blackboard.values["THIS"] == 1) {
        vdm->msg << "Start At: " << start_point << std::endl;
    }
    #endif // RFFD_USES_VATNAO

    // Loop around the frame until we reach just past where we started
    while (true) {
        i++;
        if (i == length) {
            i = 0;
            passed_zero = true;
        }

        if (still_white(border, i, was_white, num_whites, num_blacks)) {
            #ifdef RFFD_USES_VATNAO
            if (vdm != NULL && vdm->vision_debug_blackboard.values["THIS"] == 1 && was_white == false) {
                vdm->msg << "White start At: " << i << "after: " << num_blacks << " blacks" << std::endl;
            }
            #endif // RFFD_USES_VATNAO
            num_whites++;
            was_white = true;
            if (border[i] != WHITE) {
                num_blacks++;
            } else {
                num_blacks = 0;
            }
        } else {
            if (was_white) {
                #ifdef RFFD_USES_VATNAO
                if (vdm != NULL && vdm->vision_debug_blackboard.values["THIS"] == 1) {
                    vdm->msg << "Break At: " << i << " after: " << num_whites << " whites" << std::endl;
                }
                #endif // RFFD_USES_VATNAO

                int actual_num_whites = (border[i - SWITCH_RATE] == WHITE) ? num_whites : num_whites - SWITCH_RATE;
                int centre_point = i - num_whites + int(actual_num_whites/2);
                if (centre_point < 0) {
                    centre_point += length;
                }

                if (actual_num_whites > longest_white_length) {
                    second_longest_white = longest_white;
                    second_longest_white_length = longest_white_length;
                    longest_white = centre_point;
                    longest_white_length = actual_num_whites;
                } else if (actual_num_whites > second_longest_white_length) {
                    second_longest_white = centre_point;
                    second_longest_white_length = actual_num_whites;
                }
            }
            was_white = false;
            num_whites = 0;
            num_blacks++;
        }
        #ifdef RFFD_USES_VATNAO
        if (vdm != NULL && vdm->vision_debug_blackboard.values["THIS"] == 1) {
            vdm->msg << i << " ";
        }
        #endif // RFFD_USES_VATNAO

        if (i == start_point && passed_zero == true) {
            break;
        }
    }

    if (longest_white == -1 || second_longest_white == -1) {
        return;
    } else if (second_longest_white_length < longest_white_length * PERMITTED_DIFF) {
        return;
    } else if (second_longest_white_length < MIN_SIZE) {
        return;
    } else {
        border_points.push_back(longest_white);
        border_points.push_back(second_longest_white);
    }
}

Point borderPointToXY(int num_cols, int num_rows, int point) {
    if (point < num_cols) {
        return Point(point, 0);
    } else if (point < num_cols + num_rows - 1) {
        return Point(num_cols - 1, point - num_cols);
    } else if (point < num_cols * 2 + num_rows - 2) {
        return Point(num_cols - (point - num_cols - num_rows + 3), num_rows - 1);
    } else {
        return Point(0, num_rows - (point - num_cols * 2 - num_rows + 3));
    }
}

/*
Determines the field positions of each region. The rough start and end of the
line are calculated based on the direction, position and boundary of the region.
No positions are calculated for regions without directions.
*/
void RegionFieldFeatureDetector::determinePositions_(
        const VisionInfoIn& info_in, const std::vector<RegionI>& regions,
        std::vector<int8_t>& directions, std::vector<std::pair<Point, Point> >& positions, std::vector<bool>& isCurve)
{
    // Run through all the regions.
    for(unsigned int regionID=0; regionID<regions.size(); ++regionID)
    {
        // The region currently being analysed.
        const RegionI& region = regions[regionID];

        // The position of the important region point in the image.
        Point imagePos;
#ifdef RFFD_USES_VATNAO
        VisionPainter *p = NULL;
        VisionDebugQuery q;
        Point pointA;
        Point pointB;
#endif // RFFD_USES_VATNAO

#ifdef RFFD_USES_VATNAO
        if (vdm != NULL) {
            q = vdm->getQuery();
            if (q.region_index == regionID) {
                p = vdm->getGivenRegionOverlayPainter(region);
                if (q.options["Show Saliency"] == "true") {
                    p->drawColourSaliency(region);
                }
                vdm->vision_debug_blackboard.values["THIS"] = 1;
            } else {
                vdm->vision_debug_blackboard.values["THIS"] = 0;
            }
        }
#endif // RFFD_USES_VATNAO

        int num_cols = region.getCols();
        int num_rows = region.getRows();
        // Length of the border around the region. Don't count the corners twice.
        int length = num_cols * 2 + num_rows * 2 - 4;

        int x = 0;
        int y = 0;
        Colour curr;
        BorderTraverseStates state = X_INCREMENT;
        for (int i = 0; i < length; i++) {
            curr = region.getPixelColour(x, y);
            border_is_white_[i] = (curr == cWHITE);
            #ifdef RFFD_USES_VATNAO
            if (p != NULL && q.options["Show Border"] == "true") {
                if (border_is_white_[i]) {
                    p->draw(x, y, VisionPainter::YELLOW);
                } else {
                    p->draw(x, y, VisionPainter::BLUE);
                }
            }
            #endif // RFFD_USES_VATNAO

            // We need to figure out the next point around the border to go to
            std::pair<Point, BorderTraverseStates> next = nextBorderPoint(
                x, y, state, num_cols, num_rows
            );
            x = int(next.first.x());
            y = int(next.first.y());
            state = next.second;
        }

        std::vector<int> points;
        findPointsFromBorder(border_is_white_, length, points);

        if (points.size() == 2) {
            int density = region.isTopCamera() ? TOP_SALIENCY_DENSITY : BOT_SALIENCY_DENSITY;

            Point region_point_A = borderPointToXY(num_cols, num_rows, points[0]);
            Point region_point_B = borderPointToXY(num_cols, num_rows, points[1]);
            Point global_point_A = region_point_A * density + region.getBoundingBoxRaw().a;
            Point global_point_B = region_point_B * density + region.getBoundingBoxRaw().a;
            if (!region.isTopCamera()) {
                global_point_A.y() += TOP_IMAGE_ROWS;
                global_point_B.y() += TOP_IMAGE_ROWS;
            }
            Point robot_point_A = info_in.cameraToRR.pose.imageToRobotXY(global_point_A);
            Point robot_point_B = info_in.cameraToRR.pose.imageToRobotXY(global_point_B);

            isCurve[regionID] = checkIfCurved(region, region_point_A,
                                                                region_point_B);

            #ifdef RFFD_USES_VATNAO
            if (p != NULL) {
                if (q.options["Show Points"] == "true"){
                    p->draw(int(region_point_A.x()), int(region_point_A.y()), VisionPainter::ORANGE);
                    p->draw(int(region_point_B.x()), int(region_point_B.y()), VisionPainter::ORANGE);
                }
                vdm->msg << "Pos: " << points[0] << " " << points[1] << std::endl;
                vdm->msg << "Point A: " << int(region_point_A.x()) << ", " << int(region_point_A.y()) << std::endl;
                vdm->msg << "Point B: " << int(region_point_B.x()) << ", " << int(region_point_B.y()) << std::endl;
                vdm->msg << "Region: " << num_cols << ", " << num_rows << std::endl;
                vdm->msg << "Global A: " << int(global_point_A.x()) << ", " << int(global_point_A.y()) << std::endl;
                vdm->msg << "Global B: " << int(global_point_B.x()) << ", " << int(global_point_B.y()) << std::endl;
                vdm->msg << "Robot A: " << int(robot_point_A.x()) << ", " << int(robot_point_A.y()) << std::endl;
                vdm->msg << "Robot B: " << int(robot_point_B.x()) << ", " << int(robot_point_B.y()) << std::endl;

                // We set density to 8 to bold the circle
                VisionPainter *fp = vdm->getFrameOverlayPainter(8, region.isTopCamera());
                fp->drawCircle(int(global_point_A.x())/8, int(global_point_A.y())/8, 3, VisionPainter::ORANGE);
                fp->drawCircle(int(global_point_B.x())/8, int(global_point_B.y())/8, 3, VisionPainter::ORANGE);
            } else if (vdm != NULL) {
                // For any other region, draw a smaller circle in full frame
                VisionPainter *fp = vdm->getFrameOverlayPainter(8, region.isTopCamera());
                fp->drawCircle(int(global_point_A.x())/8, int(global_point_A.y())/8, 1, VisionPainter::ORANGE);
                fp->drawCircle(int(global_point_B.x())/8, int(global_point_B.y())/8, 1, VisionPainter::ORANGE);
            }
            #endif // RFFD_USES_VATNAO
            positions[regionID] = std::pair<Point, Point>(robot_point_A, robot_point_B);
        } else {
            // Set directions to -1 so we know that this region doesn't have points.
            directions[regionID] = -1;
        }

    }
}

/*
Creates lines and circles from regions with distinct directions.
*/
void RegionFieldFeatureDetector::createFeatures_(const VisionInfoIn& info_in,
    VisionInfoOut& info_out,
    const std::vector<RegionI>& regions, std::vector<int8_t>& directions,
    std::vector<std::pair<Point, Point> >& positions,
                                                     std::vector<bool>& isCurve)
{
    // Storage for the current features being built.
    std::vector<int> curFeature;
    std::vector<int> curFeature2;

    // Whether each region is already part of a feature.
    std::vector<bool> taken(regions.size(), false);

    // Lines that have been created but not verified.
    std::vector<RANSACLine> tempLines;
    tempLines.reserve(regions.size());

    // The set of valid lines found.
    std::vector<RANSACLine> validLines;
    validLines.reserve(regions.size());

    // The centre circle found.
    bool circle_found = false;
    PointF circle_centre;

    // The number of features sent to localisation. If this is too high it will
    // crash, so it must be capped.
    int features_sent = 0;

    // Try each region as a starting point for a line.
    for(unsigned int sourceRegionID=0; sourceRegionID<regions.size();
                                                               ++sourceRegionID)
    {
        // Check that this region is a line and is not already part of a
        // feature.
        if(directions[sourceRegionID] != -1 && !taken[sourceRegionID] &&
                                                       !isCurve[sourceRegionID])
        {
            // Whether a new region was added to the line this cycle.
            bool newRegionAdded = true;

            // The start and end points of the line.
            Point start = positions[sourceRegionID].first;
            Point end = positions[sourceRegionID].second;

            // Calculate the line equation of the current line.
            RANSACLine* lineEquation = new RANSACLine(start, end);

            // The current line length.
            int lineLength = DISTANCE_SQR(start, end);

            // Create a line from this region.
            curFeature.push_back(sourceRegionID);
            taken[sourceRegionID] = true;

            // Add other regions to the line as possible.
            while(newRegionAdded)
            {
                // No new region has been added yet.
                newRegionAdded = false;

                // Search for new regions to add.
                for(unsigned int regionID=0; regionID<regions.size();
                                                                     ++regionID)
                {
#ifdef RFFD_USES_VATNAO
                    if (vdm != NULL)
                    {
                        VisionDebugQuery q = vdm->getQuery();
                        if (q.region_index == sourceRegionID)
                        {
                            vdm->msg << "Distance to region " << regionID
                                << ": " << lineEquation->distance(
                                positions[regionID].first) << " / " <<
                                lineEquation->distance(
                                       positions[regionID].second) << std::endl;
                        }
                    }
#endif // RFFD_USES_VATNAO

                    if(directions[regionID] != -1 && !taken[regionID] &&
                        !isCurve[regionID] &&
                        lineEquation->distance(positions[regionID].first) <
                        MIN_LINE_CONNECTION_DISTANCE_INTERPOLATE(
                        positions[regionID].first.x()) &&
                        lineEquation->distance(positions[regionID].second) <
                        MIN_LINE_CONNECTION_DISTANCE_INTERPOLATE(
                                                positions[regionID].second.x()))
                    {
                        // Record the distances between the new points and the
                        // current line start/end.
                        int firstToStart =
                                 DISTANCE_SQR(positions[regionID].first, start);
                        int firstToEnd =
                                   DISTANCE_SQR(positions[regionID].first, end);
                        int secondToStart =
                                DISTANCE_SQR(positions[regionID].second, start);
                        int secondToEnd =
                                  DISTANCE_SQR(positions[regionID].second, end);

                        // The maximum distance away this new region can be from
                        // the current region while still being connected.
                        int firstMaxLineExtensionDistance =
                            MAX_LINE_EXTENSION_DISTANCE_INTERPOLATE(
                                       positions[regionID].first.squaredNorm());
                        int secondMaxLineExtensionDistance =
                            MAX_LINE_EXTENSION_DISTANCE_INTERPOLATE(
                                      positions[regionID].second.squaredNorm());

#ifdef RFFD_USES_VATNAO
                        if (vdm != NULL)
                        {
                            VisionDebugQuery q = vdm->getQuery();
                            if (q.region_index == sourceRegionID)
                            {
                                vdm->msg << "Distances to segments:" <<
                                                                      std::endl;
                                vdm->msg << "firstToStart: " << firstToStart <<
                                                                      std::endl;
                                vdm->msg << "firstToEnd: " << firstToEnd <<
                                                                      std::endl;
                                vdm->msg << "secondToStart: " << secondToStart
                                                                   << std::endl;
                                vdm->msg << "secondToEnd: " << secondToEnd <<
                                                                      std::endl;
                            }
                        }
#endif // RFFD_USES_VATNAO

                        // If all the points on the new line are far from the
                        // existing line don't connect it.
                        if(firstToStart > firstMaxLineExtensionDistance &&
                                firstToEnd > firstMaxLineExtensionDistance &&
                                secondToStart > secondMaxLineExtensionDistance
                                && secondToEnd > secondMaxLineExtensionDistance)
                            continue;

#ifdef RFFD_USES_VATNAO
                        if (vdm != NULL)
                        {
                            VisionDebugQuery q = vdm->getQuery();
                            if (q.region_index == sourceRegionID)
                            {
                                vdm->msg << "JOINED" << std::endl;
                            }
                        }
#endif // RFFD_USES_VATNAO

                        // Add the new line segment.
                        taken[regionID] = true;
                        newRegionAdded = true;
                        curFeature.push_back(regionID);

                        // If this is farther from the start than the old end,
                        // and is on the same side of the line as the start, it
                        // should be the new start.
                        if(firstToEnd > lineLength && firstToEnd > firstToStart)
                        {
                            // Extend the line.
                            start = positions[regionID].first;
                            lineLength = firstToEnd;

                            // Update the line equation.
                            delete lineEquation;
                            lineEquation = new RANSACLine(start, end);
                        }

                        // If this is farther from the start than the old end,
                        // and is on the same side of the line as the start, it
                        // should be the new start.
                        if(secondToEnd > lineLength && secondToEnd >
                                                                  secondToStart)
                        {
                            // Extend the line.
                            start = positions[regionID].second;
                            lineLength = secondToEnd;

                            // Update the line equation.
                            delete lineEquation;
                            lineEquation = new RANSACLine(start, end);
                        }

                        // If this is farther from the end than the old start,
                        // and is on the same side of the line as the end, it
                        // should be the new end.
                        if(firstToStart > lineLength && firstToStart >
                                                                     firstToEnd)
                        {
                            // Extend the line.
                            end = positions[regionID].first;
                            lineLength = firstToStart;

                            // Update the line equation.
                            delete lineEquation;
                            lineEquation = new RANSACLine(start, end);
                        }

                        // If this is farther from the start than the old end,
                        // and is on the same side of the line as the start, it
                        // should be the new start.
                        if(secondToStart > lineLength && secondToStart >
                                                                    secondToEnd)
                        {
                            // Extend the line.
                            end = positions[regionID].second;
                            lineLength = secondToStart;

                            // Update the line equation.
                            delete lineEquation;
                            lineEquation = new RANSACLine(start, end);
                        }
                    }
                }
            }

            // All valid regions have been added, check if the resulting line is
            // long enough.
            if(lineLength > MINIMUM_LINE_LENGTH_INTERPOLATE(
                              std::min(start.squaredNorm(), end.squaredNorm())))
            {
                // Note that this is making a copy.
                tempLines.push_back(*lineEquation);
            }
            else
            {
                // The potential line is invalid, so free up the regions.
                for(unsigned int regionID=0; regionID<curFeature.size();
                                                                     ++regionID)
                    taken[curFeature[regionID]] = false;
            }

            // Clean up variables.
            delete lineEquation;
            curFeature.clear();
        }
    }

    // Merge completed lines as possible.
    std::vector<bool> merged(tempLines.size(), false);
    merged.reserve(tempLines.size()*2);

    /*
    // This section merges the lines built above if possible, as there are
    // certain circumstances in which two sections can be build seperately and
    // not connected. As far as I am aware it is functional, but there was not
    // time to test it before comp.
    for(unsigned int sourceLine=0; sourceLine<tempLines.size(); ++sourceLine)
    {
        // Whether a new region was added to the line this cycle.
        bool newLineAdded = true;

        // The start and end points of the line.
        Point start = validLines[sourceLine].p1;
        Point end = validLines[sourceLine].p2;

        // Calculate the line equation of the current line.
        RANSACLine* lineEquation = new RANSACLine(start, end);

        // The current line length.
        int lineLength = DISTANCE_SQR(start, end);

        // Create a line from this region.
        curFeature.push_back(sourceLine);
        merged[sourceLine] = true;

        // Add other regions to the line as possible.
        while(newLineAdded)
        {
            // No new line has been added yet.
            newLineAdded = false;

            // Search for new lines to add.
            for(unsigned int line=0; line<tempLines.size(); ++line)
            {
                if(!merged[line] &&
                    lineEquation->distance(tempLines[line].p1) <
                    MIN_LINE_CONNECTION_DISTANCE_INTERPOLATE(
                    tempLines[line].p1.x()) &&
                    lineEquation->distance(tempLines[line].p2) <
                    MIN_LINE_CONNECTION_DISTANCE_INTERPOLATE(
                                                    tempLines[line].p2.x()))
                {
                    // Record the distances between the new points and the
                    // current line start/end.
                    int firstToStart =
                                    DISTANCE_SQR(tempLines[line].p1, start);
                    int firstToEnd = DISTANCE_SQR(tempLines[line].p1, end);
                    int secondToStart =
                                    DISTANCE_SQR(tempLines[line].p2, start);
                    int secondToEnd = DISTANCE_SQR(tempLines[line].p2, end);

                    // The maximum distance away this new region can be from
                    // the current region while still being connected.
                    int firstMaxLineExtensionDistance =
                        MAX_LINE_EXTENSION_DISTANCE_INTERPOLATE(
                                          tempLines[line].p1.squaredNorm());
                    int secondMaxLineExtensionDistance =
                        MAX_LINE_EXTENSION_DISTANCE_INTERPOLATE(
                                          tempLines[line].p2.squaredNorm());

                    // If all the points on the new line are far from the
                    // existing line don't connect it.
                    if(firstToStart > firstMaxLineExtensionDistance &&
                            firstToEnd > firstMaxLineExtensionDistance &&
                            secondToStart > secondMaxLineExtensionDistance
                            && secondToEnd > secondMaxLineExtensionDistance)
                        continue;

                    // Add the new line segment.
                    merged[line] = true;
                    newLineAdded = true;
                    curFeature.push_back(line);

                    // If this is farther from the start than the old end,
                    // and is on the same side of the line as the start, it
                    // should be the new start.
                    if(firstToEnd > lineLength && firstToEnd > firstToStart)
                    {
                        // Extend the line.
                        start = tempLines[line].p1;
                        lineLength = firstToEnd;

                        // Update the line equation.
                        delete lineEquation;
                        lineEquation = new RANSACLine(start, end);
                    }

                    // If this is farther from the start than the old end,
                    // and is on the same side of the line as the start, it
                    // should be the new start.
                    if(secondToEnd > lineLength && secondToEnd >
                                                              secondToStart)
                    {
                        // Extend the line.
                        start = tempLines[line].p2;
                        lineLength = secondToEnd;

                        // Update the line equation.
                        delete lineEquation;
                        lineEquation = new RANSACLine(start, end);
                    }

                    // If this is farther from the end than the old start,
                    // and is on the same side of the line as the end, it
                    // should be the new end.
                    if(firstToStart > lineLength && firstToStart >
                                                                 firstToEnd)
                    {
                        // Extend the line.
                        end = tempLines[line].p1;
                        lineLength = firstToStart;

                        // Update the line equation.
                        delete lineEquation;
                        lineEquation = new RANSACLine(start, end);
                    }

                    // If this is farther from the start than the old end,
                    // and is on the same side of the line as the start, it
                    // should be the new start.
                    if(secondToStart > lineLength && secondToStart >
                                                                secondToEnd)
                    {
                        // Extend the line.
                        end = tempLines[line].p2;
                        lineLength = secondToStart;

                        // Update the line equation.
                        delete lineEquation;
                        lineEquation = new RANSACLine(start, end);
                    }
                }
            }
        }

        // Note that this is making a copy.
        tempLines.push_back(*lineEquation);
        merged.push_back(false);

        // Clean up variables.
        delete lineEquation;
        curFeature.clear();
    }
    //*/

    // Look for a centre circle.
    for(unsigned int sourceRegionID=0; sourceRegionID<regions.size();
                                                               ++sourceRegionID)
    {
        if(directions[sourceRegionID] != -1)
        {
            // Every line corresponds to two potential centre circle centre points.
            // Try to build a circle from each, stopping if one is found.
            RANSACCircle possibleCircle(positions[sourceRegionID].first,
                    positions[sourceRegionID].second, CENTER_CIRCLE_DIAMETER/2);

            // The total length of all the lines added to each circle.
            int circle1Length = 0;
            int circle2Length = 0;

            // The total length of the curved lines added to each circle.
            int curvedLength1 = 0;
            int curvedLength2 = 0;

            // Calculate the error bounds.
            int centreCircle1Error =
                CENTRE_CIRCLE_ERROR_INTERPOLATE(
                                           possibleCircle.centre.squaredNorm());
            int centreCircle2Error = CENTRE_CIRCLE_ERROR_INTERPOLATE(
                                  possibleCircle.secondaryCentre.squaredNorm());
            int minCentreCircle1DistanceSquared = ((CENTER_CIRCLE_DIAMETER/2 -
                    centreCircle1Error) * (CENTER_CIRCLE_DIAMETER/2 -
                                                           centreCircle1Error));
            int minCentreCircle2DistanceSquared = ((CENTER_CIRCLE_DIAMETER/2 -
                    centreCircle2Error) * (CENTER_CIRCLE_DIAMETER/2 -
                                                           centreCircle2Error));
            int maxCentreCircle1DistanceSquared = ((CENTER_CIRCLE_DIAMETER/2 +
                    centreCircle1Error) * (CENTER_CIRCLE_DIAMETER/2 +
                                                           centreCircle1Error));
            int maxCentreCircle2DistanceSquared = ((CENTER_CIRCLE_DIAMETER/2 +
                    centreCircle2Error) * (CENTER_CIRCLE_DIAMETER/2 +
                                                           centreCircle2Error));

            // Check each line to see if it is part of the circle.
            for(unsigned int region=0; region<regions.size(); ++region)
            {
                if(directions[sourceRegionID] != -1)
                {
                    // The distance to each possible centre from each line end.
                    int possibleCentre1Distance1 = DISTANCE_SQR(
                                positions[region].first, possibleCircle.centre);
                    int possibleCentre1Distance2 = DISTANCE_SQR(
                               positions[region].second, possibleCircle.centre);
                    int possibleCentre2Distance1 = DISTANCE_SQR(
                        positions[region].first,
                                                possibleCircle.secondaryCentre);
                    int possibleCentre2Distance2 = DISTANCE_SQR(
                        positions[region].second,
                                                possibleCircle.secondaryCentre);

                    // The length of the line.
                    int lineLength = DISTANCE(positions[region].first,
                                                      positions[region].second);

                    // Calculate the equation for this line.
                    RANSACLine lineEquation(positions[region].first,
                                                      positions[region].second);

                    // The minimum distance from the centre.
                    int minDistanceToCentre1 = lineEquation.distance(
                                             possibleCircle.centre.cast<int>());
                    int minDistanceToCentre2 = lineEquation.distance(
                                    possibleCircle.secondaryCentre.cast<int>());

                    // If both ends of the line are on the circle, allow it.
                    if(possibleCentre1Distance1 >
                        minCentreCircle1DistanceSquared &&
                        possibleCentre1Distance1 <
                        maxCentreCircle1DistanceSquared &&
                        possibleCentre1Distance2 >
                        minCentreCircle1DistanceSquared &&
                        possibleCentre1Distance2 <
                        maxCentreCircle1DistanceSquared &&
                        minDistanceToCentre1
                                       > MIN_CENTRE_CIRCLE_DISTANCE_FROM_CENTRE)
                    {
                        curFeature.push_back(region);
                        circle1Length += lineLength;
                        if(isCurve[region])
                            curvedLength1 += lineLength;
                    }

                    if(possibleCentre2Distance1 >
                        minCentreCircle2DistanceSquared &&
                        possibleCentre2Distance1 <
                        maxCentreCircle2DistanceSquared &&
                        possibleCentre2Distance2 >
                        minCentreCircle2DistanceSquared &&
                        possibleCentre2Distance2
                        < maxCentreCircle2DistanceSquared &&
                        minDistanceToCentre2 >
                                         MIN_CENTRE_CIRCLE_DISTANCE_FROM_CENTRE)
                    {
                        curFeature2.push_back(region);
                        circle2Length += lineLength;
                        if(isCurve[region])
                            curvedLength2 += lineLength;
                    }
                }
            }

            // Check if the total length of either centre circle is enough to
            // form a valid circle.
            if(circle1Length > MIN_CENTRE_CIRCLE_LENGTH_INTERPOLATE(
                possibleCircle.centre.squaredNorm()) && curvedLength1 >
                MIN_CENTRE_CIRCLE_LENGTH_INTERPOLATE(
                                         possibleCircle.centre.squaredNorm())/2)
            {
                // Check if any of the lines can be the centre line.
                circle_found = false;
                for(unsigned int line=0; line<tempLines.size(); ++line)
                {
                    // The length of this line.
                    int lineLength = DISTANCE_SQR(tempLines[line].p1,
                                                            tempLines[line].p2);

                    // The distance of the circle centre's from the line.
                    int minDistanceToCentre = tempLines[line].distance(
                                             possibleCircle.centre.cast<int>());

                    // The distances between the line ends and the centre
                    // circle.
                    int minDistanceToLineEnd1 =
                        DISTANCE_SQR(possibleCircle.centre, tempLines[line].p1);
                    int minDistanceToLineEnd2 =
                        DISTANCE_SQR(possibleCircle.centre, tempLines[line].p2);

                    // Check if this line is long enough and close enough to be
                    // a centre line.
                    if(minDistanceToCentre <
                        MAX_CENTRE_CIRCLE_LINE_DISTANCE_FROM_CIRCLE_CENTRE &&
                        lineLength > MIN_CENTRE_CIRCLE_LINE_LENGTH &&
                        (minDistanceToLineEnd1 <
                        MAX_CENTRE_CIRCLE_LINE_END_DISTANCE ||
                        minDistanceToLineEnd2 <
                        MAX_CENTRE_CIRCLE_LINE_END_DISTANCE ||
                        (minDistanceToLineEnd1 < lineLength &&
                                           minDistanceToLineEnd2 < lineLength)))
                    {
                        circle_centre = possibleCircle.centre;
                        float distance = sqrt(circle_centre.x() *
                                circle_centre.x() + circle_centre.y() *
                                                             circle_centre.y());
                        float heading = atan2(circle_centre.y(),
                                                             circle_centre.x());
                        RRCoord c = RRCoord(distance, heading);
                        CentreCircleInfo l = CentreCircleInfo();
                        FieldFeatureInfo f = FieldFeatureInfo(c, l);

                        Point centreRight = tempLines[line].p1;
                        Point centreLeft = tempLines[line].p2;
                        if (centreRight.y() > centreLeft.y()) {
                           centreRight = tempLines[line].p2;
                           centreLeft = tempLines[line].p1;
                        }
                        float centreLineAngle = atan2(centreRight.y() -
                              centreLeft.y(), centreRight.x() - centreLeft.x());

                        Point directionRight = Point(0,0);
                        Point directionLeft = f.rr.toCartesian();
                        if (directionRight.y() > directionLeft.y()) {
                           directionRight = f.rr.toCartesian();
                           directionLeft = Point(0,0);
                        }
                        float robotCentreAngle = atan2(directionRight.y() -
                            directionLeft.y(), directionRight.x() -
                                                             directionLeft.x());
                        float angle = robotCentreAngle - centreLineAngle;

                        if (angle < 0) {
                           angle += M_PI;
                        }

                        f.rr.orientation() = angle;

                        ++features_sent;
                        info_out.features.push_back(f);
                        if(features_sent >= MAX_LOCALISATION_FEATURES)
                            return;

                        // Make sure the centre line is made valid.
                        LineInfo l1(tempLines[line].p1, tempLines[line].p2);
                        RRCoord c1(0, 0);
                        FieldFeatureInfo f1 = FieldFeatureInfo(c1, l1);
                        info_out.features.push_back(f1);
                        validLines.push_back(tempLines[line]);
                        merged[line] = true;

                        // Stop searching for centre circles: there is only one
                        // to find.
                        circle_found = true;
                        break;
                    }
                }

                // If a circle was found stop searching for centre circles:
                // there is only one to find.
                if(circle_found)
                {std::cout << "Centre circle" << std::endl;
                    break;
                }
            }
            else if(circle2Length > MIN_CENTRE_CIRCLE_LENGTH_INTERPOLATE(
                possibleCircle.secondaryCentre.squaredNorm()) && curvedLength2 >
                MIN_CENTRE_CIRCLE_LENGTH_INTERPOLATE(
                                possibleCircle.secondaryCentre.squaredNorm())/2)
            {
                // Check if any of the lines can be the centre line.
                circle_found = false;
                for(unsigned int line=0; line<tempLines.size(); ++line)
                {
                    // The length of this line.
                    int lineLength = DISTANCE_SQR(tempLines[line].p1,
                                                            tempLines[line].p2);

                    // The distance of the circle centre's from the line.
                    int minDistanceToCentre = tempLines[line].distance(
                                    possibleCircle.secondaryCentre.cast<int>());

                    // The distances between the line ends and the centre
                    // circle.
                    int minDistanceToLineEnd1 =
                        DISTANCE_SQR(possibleCircle.secondaryCentre,
                                                            tempLines[line].p1);
                    int minDistanceToLineEnd2 =
                        DISTANCE_SQR(possibleCircle.secondaryCentre,
                                                            tempLines[line].p2);

                    // Check if this line is long enough and close enough to be
                    // a centre line.
                    if(minDistanceToCentre <
                        MAX_CENTRE_CIRCLE_LINE_DISTANCE_FROM_CIRCLE_CENTRE &&
                        lineLength > MIN_CENTRE_CIRCLE_LINE_LENGTH &&
                        (minDistanceToLineEnd1 <
                        MAX_CENTRE_CIRCLE_LINE_END_DISTANCE ||
                        minDistanceToLineEnd2 <
                        MAX_CENTRE_CIRCLE_LINE_END_DISTANCE ||
                        (minDistanceToLineEnd1 < lineLength &&
                                           minDistanceToLineEnd2 < lineLength)))
                    {
                        circle_centre = possibleCircle.secondaryCentre;
                        float distance = sqrt(circle_centre.x() *
                                circle_centre.x() + circle_centre.y() *
                                                             circle_centre.y());
                        float heading = atan2(circle_centre.y(),
                                                             circle_centre.x());
                        RRCoord c = RRCoord(distance, heading);
                        CentreCircleInfo l = CentreCircleInfo();
                        FieldFeatureInfo f = FieldFeatureInfo(c, l);

                        Point centreRight = tempLines[line].p1;
                        Point centreLeft = tempLines[line].p2;
                        if (centreRight.y() > centreLeft.y()) {
                           centreRight = tempLines[line].p2;
                           centreLeft = tempLines[line].p1;
                        }
                        float centreLineAngle = atan2(centreRight.y() -
                              centreLeft.y(), centreRight.x() - centreLeft.x());

                        Point directionRight = Point(0,0);
                        Point directionLeft = f.rr.toCartesian();
                        if (directionRight.y() > directionLeft.y()) {
                           directionRight = f.rr.toCartesian();
                           directionLeft = Point(0,0);
                        }
                        float robotCentreAngle = atan2(directionRight.y() -
                            directionLeft.y(), directionRight.x() -
                                                             directionLeft.x());
                        float angle = robotCentreAngle - centreLineAngle;

                        if (angle < 0) {
                           angle += M_PI;
                        }

                        f.rr.orientation() = angle;

                        ++features_sent;
                        info_out.features.push_back(f);
                        if(features_sent >= MAX_LOCALISATION_FEATURES)
                            return;

                        // Make sure the centre line is made valid.
                        LineInfo l1(tempLines[line].p1, tempLines[line].p2);
                        RRCoord c1(0, 0);
                        FieldFeatureInfo f1 = FieldFeatureInfo(c1, l1);
                        info_out.features.push_back(f1);
                        validLines.push_back(tempLines[line]);
                        merged[line] = true;

                        // Stop searching for centre circles: there is only one
                        // to find.
                        circle_found = true;
                        break;
                    }
                }

                // If a circle was found stop searching for centre circles:
                // there is only one to find.
                if(circle_found)
                {std::cout << "Centre circle" << std::endl;
                    break;
                }
            }
        }
    }

    // Keep valid lines.
    for(unsigned int line=0; line<tempLines.size(); ++line)
    {
        if(!merged[line] && (!circle_found || (DISTANCE_SQR(tempLines[line].p1,
            circle_centre) > MIN_DISTANCE_TO_CENTRE_CIRCLE && DISTANCE_SQR(
            tempLines[line].p2, circle_centre) >
                                                MIN_DISTANCE_TO_CENTRE_CIRCLE)))
        {
            LineInfo l1(tempLines[line].p1, tempLines[line].p2);
            RRCoord c1(0, 0);
            FieldFeatureInfo f1 = FieldFeatureInfo(c1, l1);
            info_out.features.push_back(f1);
            validLines.push_back(tempLines[line]);
        }
    }

    // Generate corners and T intersections from line intersects.
    std::vector<bool> hasT(validLines.size(), false);
    std::vector<bool> hasCorner(validLines.size(), false);
    std::vector<FieldFeatureInfo> foundT(validLines.size());
    std::vector<FieldFeatureInfo> foundCorner(validLines.size());
    std::vector<Point> foundTPoint(validLines.size());
    std::vector<Point> foundCornerPoint(validLines.size());
    std::vector<int> otherLineCorner(validLines.size());
    std::vector<int> otherLineT(validLines.size());
    for(unsigned int sourceLine=0; sourceLine<validLines.size(); ++sourceLine)
    {
        // Calculate the relevant attributes of the source line.
        float sourceLineAngle = validLines[sourceLine].getAngle();
        int sourceLineLength = DISTANCE_SQR(validLines[sourceLine].p1,
                                                     validLines[sourceLine].p2);
        if(sourceLineAngle < 0)
            sourceLineAngle += M_PI;

        // Whether this line has a T and corner for goal box purposes.
        for(unsigned int line=sourceLine+1; line<validLines.size(); ++line)
        {
            // The angle to this line.
            float lineAngle = validLines[line].getAngle();
            if(lineAngle < 0)
                lineAngle += M_PI;

            float angleBetween;
            if(lineAngle > sourceLineAngle)
                angleBetween = lineAngle - sourceLineAngle;
            else
                angleBetween = sourceLineAngle - lineAngle;

            // Determine if the lines are roughly perpendicular.
            if(fabs(angleBetween - (M_PI/2.0f)) < MAX_PERPENDICULAR_ANGLE_ERROR)
            {
                // Find the intersection between these two lines. As we know
                // they're pretty perpendicular there should be one.
                Point intersection = validLines[sourceLine].getIntersect(
                                                              validLines[line]);

                // Calculate the distances between the line ends and the
                // intersection.
                int sourceToIntersection1 =
                          DISTANCE_SQR(validLines[sourceLine].p1, intersection);
                int sourceToIntersection2 =
                          DISTANCE_SQR(validLines[sourceLine].p2, intersection);
                int otherToIntersection1 =
                                DISTANCE_SQR(validLines[line].p1, intersection);
                int otherToIntersection2 =
                                DISTANCE_SQR(validLines[line].p2, intersection);
                int otherLineLength =
                         DISTANCE_SQR(validLines[line].p1, validLines[line].p2);

                // Whether the intersection is on the known portion of each
                // line.
                bool onSourceLine = sourceToIntersection1 < sourceLineLength &&
                    sourceToIntersection2 < sourceLineLength &&
                    sourceToIntersection1 > T_INTERSECTION_EXCESS &&
                                  sourceToIntersection2 > T_INTERSECTION_EXCESS;
                bool onOtherLine = otherToIntersection1 < otherLineLength &&
                    otherToIntersection2 < otherLineLength &&
                    otherToIntersection1 > T_INTERSECTION_EXCESS &&
                                   otherToIntersection2 > T_INTERSECTION_EXCESS;

                // Check the other line is long enough to form features.
                int minFeatureLineLength =
                    MIN_FEATURE_LINE_LENGTH_INTERPOLATE(
                                                    intersection.squaredNorm());

                // The maximum distance from the intersection the closest
                // line end can be.
                int maxLineEndIntersectionDistance =
                    MAX_LINE_END_INTERSECTION_DISTANCE_INTERPOLATE(
                                                    intersection.squaredNorm());

/*
                std::cout << "onSourceLine: " << onSourceLine << std::endl;
                std::cout << "sourceLineLength: " << sqrt(sourceLineLength)
                                                               << std::endl;
                std::cout << "sourceToIntersection1: " <<
                                   sqrt(sourceToIntersection1) << std::endl;
                std::cout << "sourceToIntersection2: " <<
                                   sqrt(sourceToIntersection2) << std::endl;
                std::cout << "otherToIntersection1: " <<
                    (otherToIntersection1 < maxLineEndIntersectionDistance)
                                                               << std::endl;
                std::cout << "otherToIntersection2: " <<
                    (otherToIntersection2 < maxLineEndIntersectionDistance)
                                                               << std::endl;
                std::cout << "onOtherLine: " << onOtherLine << std::endl;
                std::cout << "otherLineLength: " << sqrt(otherLineLength) <<
                                                                  std::endl;
                std::cout << "otherToIntersection1: " <<
                                    sqrt(otherToIntersection1) << std::endl;
                std::cout << "otherToIntersection2: " <<
                                    sqrt(otherToIntersection2) << std::endl;
                std::cout << "sourceToIntersection1: " <<
                    (sourceToIntersection1 < maxLineEndIntersectionDistance)
                                                               << std::endl;
                std::cout << "sourceToIntersection2: " <<
                    (sourceToIntersection2 < maxLineEndIntersectionDistance)
                                                               << std::endl;
                std::cout << "intersection: (" << intersection.x() << ", "
                                    << intersection.y() << ")" << std::endl;
                std::cout << "source p1: (" << validLines[sourceLine].p1.x()
                    << ", " << validLines[sourceLine].p1.y() << ")" <<
                                                                  std::endl;
                std::cout << "source p2: (" << validLines[sourceLine].p2.x()
                    << ", " << validLines[sourceLine].p2.y() << ")" <<
                                                                  std::endl;
                std::cout << "source equation: " <<
                    validLines[sourceLine].t1 << "x + " <<
                    validLines[sourceLine].t2 << "y + " <<
                                     validLines[sourceLine].t3 << std::endl;
                std::cout << "other p1: (" << validLines[line].p1.x() <<
                        ", " << validLines[line].p1.y() << ")" << std::endl;
                std::cout << "other p2: (" << validLines[line].p2.x() <<
                        ", " << validLines[line].p2.y() << ")" << std::endl;
                std::cout << "other equation: " << validLines[line].t1 <<
                    "x + " << validLines[line].t2 <<  "y + " <<
                                           validLines[line].t3 << std::endl;
//*/

                // Check for T intersections. T intersections are on one of
                // the lines and near the end of the other.
                if(!(onSourceLine && onOtherLine) && ((onSourceLine &&
                    (otherToIntersection1 < maxLineEndIntersectionDistance
                    || otherToIntersection2 < maxLineEndIntersectionDistance))
                    || (onOtherLine && (sourceToIntersection1 <
                    maxLineEndIntersectionDistance || sourceToIntersection2 <
                                              maxLineEndIntersectionDistance))))
                {
                    // Build a T intersection at the relevant position.
                    float angle = findTAngle(intersection, (onSourceLine ?
                                    validLines[line] : validLines[sourceLine]));
                    float distance = sqrt(intersection.x() *
                            intersection.x() + intersection.y() *
                                                              intersection.y());
                    float heading = atan2(intersection.y(), intersection.x());
                    RRCoord r = RRCoord(distance, heading, -angle);
                    Point intersectionImage =
                          info_in.cameraToRR.convertToImageXY(intersection);
                    TJunctionInfo t = TJunctionInfo(intersectionImage);
                    FieldFeatureInfo f = FieldFeatureInfo(r, t);

                    // Create the feature if one was found.
                    if(sourceLineLength > minFeatureLineLength &&
                                         otherLineLength > minFeatureLineLength)
                    {
                        ++features_sent;
                        info_out.features.push_back(f);
                        if(features_sent >= MAX_LOCALISATION_FEATURES)
                            return;
                    }
                    // Otherwise record the corner for this line.
                    // TODO: Allow for multiple found corners.
                    else if(sourceLineLength > minFeatureLineLength ||
                                         otherLineLength > minFeatureLineLength)
                    {
                        if(sourceLineLength <= minFeatureLineLength)
                        {
                            hasT[sourceLine] = true;
                            foundT[sourceLine] = f;
                            foundTPoint[sourceLine] = intersection;
                        }
                        else
                        {
                            hasT[line] = true;
                            foundT[line] = f;
                            foundTPoint[line] = intersection;
                        }
                    }
                }

                // If this isn't a T intersection, check for a corner.
                else if(!(onSourceLine || onOtherLine) &&
                    ((sourceToIntersection1 < maxLineEndIntersectionDistance
                    || sourceToIntersection2 < maxLineEndIntersectionDistance)
                    && (otherToIntersection1 < maxLineEndIntersectionDistance ||
                        otherToIntersection2 < maxLineEndIntersectionDistance)))
                {
                    // Build a corner at the relevant position.

                    // Determine the robot relative coordinates.
                    Point e1, e2;
                    float distance = sqrt(intersection.x() *
                        intersection.x() + intersection.y() *
                                                          intersection.y());
                    float heading = atan2(intersection.y(),
                                                          intersection.x());

                    // Determine the robot relative heading relative angle.
                    float angle = findCAngle(intersection,
                                  validLines[sourceLine], validLines[line]);

                    // Determine the image relative location of the feature.
                    Point intersectionImage =
                          info_in.cameraToRR.convertToImageXY(intersection);
                    e1 = info_in.cameraToRR.convertToImageXY(e1);
                    e2 = info_in.cameraToRR.convertToImageXY(e2);

                    // Create the final feature.
                    RRCoord r = RRCoord(distance, heading, -angle);
                    CornerInfo c = CornerInfo(intersectionImage, e1, e2);
                    FieldFeatureInfo f = FieldFeatureInfo(r, c);

                    // Create the feature if one was found.
                    if(sourceLineLength > minFeatureLineLength &&
                                         otherLineLength > minFeatureLineLength)
                    {
                        ++features_sent;
                        info_out.features.push_back(f);
                        if(features_sent >= MAX_LOCALISATION_FEATURES)
                            return;
                    }
                    // Otherwise record the corner for this line.
                    // TODO: Allow for multiple found corners.
                    else if(sourceLineLength > minFeatureLineLength ||
                                         otherLineLength > minFeatureLineLength)
                    {
                        if(sourceLineLength <= minFeatureLineLength)
                        {
                            hasCorner[sourceLine] = true;
                            foundCorner[sourceLine] = f;
                            foundCornerPoint[sourceLine] = intersection;
                        }
                        else
                        {
                            hasCorner[line] = true;
                            foundCorner[line] = f;
                            foundCornerPoint[line] = intersection;
                        }
                    }
                }
            }
        }

        // Check whether this seems to be a goal box line.
        if(hasT[sourceLine] && hasCorner[sourceLine] && abs(DISTANCE(
            foundCornerPoint[sourceLine], foundTPoint[sourceLine])
                                        - GOAL_BOX_LENGTH) < MAX_GOAL_BOX_ERROR)
        {
            ++features_sent;
            info_out.features.push_back(foundCorner[sourceLine]);
            if(features_sent >= MAX_LOCALISATION_FEATURES)
                return;
            ++features_sent;
            info_out.features.push_back(foundT[sourceLine]);
            if(features_sent >= MAX_LOCALISATION_FEATURES)
                return;
        }
    }
//*/
}

/*
Interpolates between hyperparameter values based on distance.
*/
inline int RegionFieldFeatureDetector::interpolateHyperparameters(
    const long long distance, const long long min, const long long max,
                     const long long min_distance, const long long max_distance)
{
    // First check the thresholds.
    if(distance < min_distance)
        return(min);
    else if(distance > max_distance)
        return(max);

    // Interpolate between min and max.
    else
    {
        return(min +
             (((max-min)*(distance-min_distance))/(max_distance-min_distance)));
    }
}


// IMPLEMENTATIONS FROM THE OLD SYSTEM. TODO: Replace with nicer methods.
float RegionFieldFeatureDetector::findTAngle(Point &p, RANSACLine &l) {
   float angle = findGradient(l, p);
   float theta = atan2(p.x(), p.y());

   if (angle > 0) {
      angle = theta + DEG2RAD(180) - angle;
   } else {
      angle = theta - (DEG2RAD(180) + angle);
   }

   if (angle > M_PI) {
      angle = NORMALISE(angle);
   }
   return angle;
}

float RegionFieldFeatureDetector::findCAngle(Point &p, RANSACLine &l1, RANSACLine &l2) {
   float g1 = findGradient(l1, p);
   float g2 = findGradient(l2, p);
   float angle = (g1+g2)/2;
   float roughQuadrantLimit = (8*(M_PI/18));
   if ((g1 > 0) && (g2 < 0) &&
         (g1 > roughQuadrantLimit) && (g2 < -roughQuadrantLimit)) {
      g2 += 2*M_PI;
      angle = (g1+g2)/2;
   }
   if ((g2 > 0) && (g1 < 0) &&
         (g2 > roughQuadrantLimit) && (g1 < -roughQuadrantLimit)) {
      g1 += 2*M_PI;
      angle = (g1+g2)/2;
   }
   if (angle > M_PI) angle -= 2*M_PI;

   float theta = atan2(p.x(), p.y());

   if (angle > 0) {
      angle = theta + M_PI - angle;
   } else {
      angle = theta - (M_PI + angle);
   }

   if (angle > M_PI) {
      angle = fmod(angle - M_PI, 2 * M_PI) + ((angle > 0) ? -M_PI : M_PI);
   }
   return angle;
}

float RegionFieldFeatureDetector::findGradient(RANSACLine &l, Point &p) {
   // Work out which direction to find gradient in
   float distp1 = DISTANCE_SQR(p, l.p1);
   float distp2 = DISTANCE_SQR(p, l.p2);
   Point far;
   Point close;
   if (distp1 > distp2) {
      far = l.p1;
      close = l.p2;
   } else {
      far = l.p2;
      close = l.p1;
   }
   return (atan2(far.x() - close.x(), far.y() - close.y()));
}

bool RegionFieldFeatureDetector::checkIfCurved(const RegionI& region,
                                 const Point& startPoint, const Point& endPoint)
{
    // Calculate the line equation.
    RANSACLine baseLine(startPoint, endPoint);
    RANSACLine perpendicularLine;

    // The width and height.
    int width = region.getCols();
    int height = region.getRows();

    // Calculate the line centre point.
    Point centre = (endPoint + startPoint)/2;

    // The current point being considered.
    Point curPoint = centre;

    // The extremes of the lines.
    int leftOffset = -1, rightOffset = -1;

    // Create the perpendicular line equation.
    perpendicularLine.t1 = baseLine.t2;
    perpendicularLine.t2 = baseLine.t1;
    perpendicularLine.sqrtt1t2 = baseLine.sqrtt1t2;

    // Check if the centre point of the line is accurate or offset.

    // Calculate t3 of the perpendicular line at the centre point.
    perpendicularLine.t3 = - perpendicularLine.t1*centre.x() -
                                                perpendicularLine.t2*centre.x();

    // Starting from centre move out looking for the line edges.

    // Moving left.
    bool goingUp = (perpendicularLine.t2 == 0) ||
                             (-perpendicularLine.t1 / perpendicularLine.t2) < 0;
    int offset = 0;
    while(leftOffset == -1)
    {
        // The y value for this column.
        int colY = curPoint.y();
        if(perpendicularLine.t2 != 0)
        {
            colY = (-perpendicularLine.t1 * curPoint.x() - perpendicularLine.t3)
                                                         / perpendicularLine.t2;
        }

        // If this pixel isn't white the edge has been found.
        if(region.getPixelColour(curPoint.x(), curPoint.y()) != cWHITE)
            leftOffset = offset;

        // Otherwise determine the next pixel.
        else
        {
            if(goingUp)
            {
                ++offset;
                if(curPoint.y() > colY)
                    --curPoint.y();
                else
                    --curPoint.x();

                // Check if the edge of the region has been reached.
                if(curPoint.x() < 0 || curPoint.y() < 0)
                    break;
            }
            else
            {
                ++offset;
                if(curPoint.y() < colY)
                    ++curPoint.y();
                else
                    --curPoint.x();

                // Check if the edge of the region has been reached.
                if(curPoint.x() < 0 || (goingUp && curPoint.y() < 0) ||
                                           (!goingUp && curPoint.y() >= height))
                    break;
            }
        }
    }

#ifdef RFFD_USES_VATNAO
    Point firstPoint = curPoint;
#endif // RFFD_USES_VATNAO

    // Moving right.
    curPoint = centre;
    offset = 0;
    while(rightOffset == -1)
    {
        // The y value for this column.
        int colY = curPoint.y();
        if(perpendicularLine.t2 != 0)
        {
            colY = (-perpendicularLine.t1 * curPoint.x() - perpendicularLine.t3)
                                                         / perpendicularLine.t2;
        }

        // If this pixel isn't white the edge has been found.
        if(region.getPixelColour(curPoint.x(), curPoint.y()) != cWHITE)
            rightOffset = offset;

        // Otherwise determine the next pixel.
        else
        {
            if(goingUp)
            {
                ++offset;
                if(curPoint.y() > colY)
                    --curPoint.y();
                else
                    ++curPoint.x();

                // Check if the edge of the region has been reached.
                if(curPoint.x() < 0 || curPoint.y() < 0)
                    break;
            }
            else
            {
                ++offset;
                if(curPoint.y() < colY)
                    ++curPoint.y();
                else
                    ++curPoint.x();

                // Check if the edge of the region has been reached.
                if(curPoint.x() >= width || (goingUp && curPoint.y() < 0) ||
                                           (!goingUp && curPoint.y() >= height))
                    break;
            }
        }
    }

#ifdef RFFD_USES_VATNAO
    VisionPainter *p = NULL;
    VisionDebugQuery q;

    if (vdm != NULL && vdm->vision_debug_blackboard.values["THIS"] == 1) {
        q = vdm->getQuery();
        p = vdm->getGivenRegionOverlayPainter(region);
        if (q.options["Show Saliency"] == "true") {
            p->drawColourSaliency(region);
            p->draw(firstPoint.x(), firstPoint.y(), VisionPainter::YELLOW);
            p->draw(firstPoint.x(), firstPoint.y(), VisionPainter::BLUE);
        }
    }
#endif // RFFD_USES_VATNAO

    // Determine if the line is curved.

    // First pixel isn't white or both sides go over the edge.
    int diff = abs(leftOffset - rightOffset);
    diff += diff/2;
    if(leftOffset == 0 || rightOffset == 0)
        return(true);

    else if(leftOffset == -1 || rightOffset == -1)
        return(false);

    // Left offset is too different from right offset.
    else if(diff / leftOffset > 1 || diff / rightOffset > 1)
        return(true);

    // Otherwise this is a line.
    else
        return(false);

/*
    // The width of the region.
    const int width = region.getCols();
    const int height = region.getRows();

    // The change in pixel value for each half.
    int firstHalfChange = 0;
    int secondHalfChange = 0;

    // First and last white pixel in each row.
    std::vector<std::pair<int, int> > firstLastHoriz(region.getRows(),
                                                   std::make_pair(width+1, -1));

    // First and last white pixel in each column.
    std::vector<std::pair<int, int> > firstLastVert(region.getCols(),
                                                  std::make_pair(height+1, -1));

    // The current column.
    int column = 0;

    // The current row.
    int row = 0;

    // Run through the pixels within the region.
    const RegionI::iterator_fovea end = region.end_fovea();
    for(RegionI::iterator_fovea it = region.begin_fovea(); it < end; ++it)
    {
        // Next row.
        if(column == width)
        {
            column = 0;
            ++row;
        }

        // Find the first and last white pixel in this row.
        if(it.colour() == cWHITE)
        {
            if(firstLastHoriz[row].first < column)
                firstLastHoriz[row].first = column;
            if(firstLastHoriz[row].second < column)
                firstLastHoriz[row].second = column;
            if(firstLastVert[column].first < row)
                firstLastVert[column].first = row;
            if(firstLastVert[column].second < row)
                firstLastVert[column].second = row;
        }

        // Next column.
        ++column;
    }

    // Check that the rate of change of the first half of the line is similar to
    // the rate of change of the second half.

    //

    for(int col=1; col<width/2; ++col)
        firstHalfChange += firstLastHoriz[col] - firstLastHoriz[col-1];
    for(int col=1; col<width/2; ++col)
        firstHalfChange += firstLastHoriz[col] - firstLastHoriz[col-1];
    */
}
