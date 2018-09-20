#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

#include "perception/vision/Fovea.hpp"

//#define FOVEA_TIMINGS
#ifdef FOVEA_TIMINGS
#include "utils/Timer.hpp"
#endif // FOVEA_TIMINGS

/**
 * Free the _colour, _grey and _edge arrays.
 */
Fovea::~Fovea()
{//*
    // Clear existing child fovea.
    for(unsigned int fovea=0; fovea<child_fovea_.size(); ++fovea)
        delete child_fovea_[fovea];
    child_fovea_.clear();

    // Clear data array.
    if(hasColour)
        delete[] _colour;
    if(hasGrey)
        delete[] _grey;
    if(hasEdge)
        delete[] _edge;
    //*/
}


/**
 * Gets a pointer to the first item in the colour array.
 */
const Colour* Fovea::getColourArray()
{
    return _colour;
}

/**
 * Gets a pointer to the first item in the raw image.
 */
const uint8_t* Fovea::getRawPixelArray()
{
    return _rawImage;
}

/**
 * Updates the fovea to use the data in combined_frame and colour classification
 * of colour_classifier.
 */
void Fovea::generate(const CombinedFrame& combined_frame,
        const ColourClassifier& colour_classifier)
{
#ifdef FOVEA_TIMINGS
    static int startStopTime = 0;
    static int colourTime = 0;
    static int blurTime = 0;
    static int edgeTime = 0;
    static int frameCount = 0;

    if(bb.width() == TOP_SALIENCY_COLS)
        ++frameCount;

    Timer timer;
    timer.restart();
#endif // FOVEA_TIMINGS

    // Clear existing child fovea.
    for(unsigned int fovea=0; fovea<child_fovea_.size(); ++fovea)
        delete child_fovea_[fovea];
    child_fovea_.clear();

    // Record combined_frame and colour_classifier in case child fovea need
    // them.
    combined_frame_ = &combined_frame;
    colour_classifier_ = &colour_classifier;

    // Record the frame.
    if(top)
        _rawImage = combined_frame.top_frame_;
    else
        _rawImage = combined_frame.bot_frame_;

    // Translate the y axis stop coordinates into linear stop start coordinates.
    std::pair<std::vector<int>*, std::vector<const uint8_t*>* > startStop =
                                                                getStartStop_();

#ifdef FOVEA_TIMINGS
    if(bb.width() == TOP_SALIENCY_COLS)
        startStopTime += timer.elapsed_us();
    timer.restart();
#endif // FOVEA_TIMINGS

    // Generate colour image if needed.
    if(hasColour)
    {
        if(top)
        {
            makeColourTop_(combined_frame, colour_classifier, *startStop.first,
                                                             *startStop.second);
        }
        else
        {
            makeColourBot_(combined_frame, colour_classifier, *startStop.first,
                                                             *startStop.second);
        }
    }

#ifdef FOVEA_TIMINGS
    if(bb.width() == TOP_SALIENCY_COLS)
        colourTime += timer.elapsed_us();
    timer.restart();
#endif // FOVEA_TIMINGS

    // Generate grey image if needed.
    if(hasGrey)
        blurGrey_();

#ifdef FOVEA_TIMINGS
    if(bb.width() == TOP_SALIENCY_COLS)
        blurTime += timer.elapsed_us();
    timer.restart();
#endif // FOVEA_TIMINGS

    // Generate edge image if needed.
    if(hasEdge)
        makeEdge_();

#ifdef FOVEA_TIMINGS
    if(bb.width() == TOP_SALIENCY_COLS)
        edgeTime += timer.elapsed_us();
#endif // FOVEA_TIMINGS

    // Clean up x start stop.
    delete startStop.first;
    delete startStop.second;

#ifdef FOVEA_TIMINGS
    if(frameCount == 1000)
    {
        std::cout << "1000 frame average" << std::endl << "Width: " << bb.width() << " Height: " << bb.height() << std::endl;
        std::cout << "Start stop time: " << startStopTime/frameCount << std::endl;
        std::cout << "Colour classification: " << colourTime/frameCount << std::endl;
        std::cout << "Grey blur: " << blurTime/frameCount << std::endl;
        std::cout << "Edge detection: " << edgeTime/frameCount << std::endl << std::endl;
        startStopTime = 0;
        colourTime = 0;
        blurTime = 0;
        edgeTime = 0;
        frameCount = 0;
    }
#endif // FOVEA_TIMINGS
}

/*
 * Translates the y axis robot part stop array to a linear start stop array.
 */
std::pair<std::vector<int>*, std::vector<const uint8_t*>* >
                                                    Fovea::getStartStop_() const
{
    // The number of columns in the image.
    const int doubleCols = 2*(top*TOP_IMAGE_COLS+(!top)*BOT_IMAGE_COLS);

    // Create a startStop array.
    std::vector<int>* startStop = new std::vector<int>();
    std::vector<const uint8_t*>* startStopRaw =
                                              new std::vector<const uint8_t*>();
    startStop->reserve(bb.height()*4);

    // The number of x and y axis rows covered by this fovea.
    int xAxisCols = bb.width();
    int yAxisRows = bb.height();

    // The y axis stopping points relevant to this fovea, sorted by height.
    // Stored as y, x, in descending order.
    std::vector<std::pair<int, int> > sortedStops;

    // The set of starts and stops relevant at this y value.
    std::vector<int> relevantStartStops;

    // Iterator to the next y value to include.
    std::vector<std::pair<int, int> >::iterator highestY;

    // The first pixel in the fovea in the raw image.
    const uint8_t* firstPixel = _rawImage + bb.a.x()*density*2 +
                                                    bb.a.y()*density*doubleCols;

    // Create the sorted y axis stop array.
    sortedStops.reserve(xAxisCols);
    if(top)
    {
        for(int x=0; x<xAxisCols; ++x)
        {
            sortedStops.push_back(std::make_pair(
                std::max(combined_frame_->camera_to_rr_.getTopEndScanCoord(x*density)/density, 0),
                                                                            x));
        }
    }
    else
    {
        for(int x=0; x<xAxisCols; ++x)
        {
            sortedStops.push_back(std::make_pair(
                std::max(combined_frame_->camera_to_rr_.getBotEndScanCoord(x*density)/density, 0),
                                                                            x));
        }
    }
    std::sort(sortedStops.begin(), sortedStops.end());

    // Initialise the highestY pointer.
    highestY = sortedStops.begin();

    // Set up relevantStartStops.
    relevantStartStops.reserve(10);
    relevantStartStops.push_back(0);
    relevantStartStops.push_back(bb.width());

    // Build the startStop array.
    for(int y=0; y<yAxisRows; ++y)
    {
        // The linear location of the start of this row.
        int startVal = y*bb.width();
        const uint8_t* startValRaw = firstPixel+y*density*doubleCols;

        // Firstly, check for new y stops to include.
        while(highestY < sortedStops.end() && (*highestY).first <= y)
        {
            // A new stop needs to be added, work out where it goes.
            int newX = (*highestY).second;
            bool stopPoint = false; // Start stop of regular classification.
            for(unsigned int point=0; point<relevantStartStops.size(); ++point)
            {
                // Check if newX should extend the robot block to the right.
                if(!stopPoint && newX == relevantStartStops[point])
                {
                    // Skip as long as we're not at the end, the y value is
                    // the same, we haven't gone past the next point, and the
                    // next point is next to this one.
                    while(highestY < sortedStops.end()-1 &&
                            (*(highestY+1)).first == (*highestY).first &&
                            (point+1 >= relevantStartStops.size() ||
                            ((*(highestY+1)).second <
                            relevantStartStops[point+1] &&
                               (*(highestY+1)).second == (*highestY).second+1)))
                        ++highestY;
                    relevantStartStops[point] = (*highestY).second+1;

                    // Once the point is added break from the loop.
                    break;
                }

                // Otherwise check if newX should extend a robot block to the
                // left.
                else if(stopPoint && newX < relevantStartStops[point])
                {
                    // Firstly find the size of this group and check if it
                    // connects to the right.
                    int maxX[2];
                    maxX[0] = newX;
                    maxX[1] = newX;

                    // Skip as long as we're not at the end, the y value is
                    // the same, we haven't gone past the next point, and the
                    // next point is next to this one.
                    while(highestY < sortedStops.end()-1 &&
                            (*(highestY+1)).first == (*highestY).first &&
                            (point+1 >= relevantStartStops.size() ||
                            ((*(highestY+1)).second < relevantStartStops[point]
                            && (*(highestY+1)).second == (*highestY).second+1)))
                        ++highestY;
                    maxX[1] = (*highestY).second+1;

                    // Check for merging with the group on the right.
                    if(maxX[1] == relevantStartStops[point])
                    {
                        // Extend the group to the left.
                        relevantStartStops[point] = newX;

                        // Once the point is added break from the loop.
                        break;
                    }

                    // This group doesn't connect with an existing group, so
                    // insert a new one.
                    relevantStartStops.insert(relevantStartStops.begin()+point,
                                                                  maxX, maxX+2);

                    // Once the point is added break from the loop.
                    break;
                }

                // Toggle whether this is a start point.
                stopPoint = !stopPoint;
            }

            // Lastly check if any two groups should be merged.
            for(unsigned int point=2; point<relevantStartStops.size()-2;
                                                                       point+=2)
            {
                if(relevantStartStops[point] == relevantStartStops[point+1])
                {
                    // The groups can be merged. Just delete these two values.
                    relevantStartStops.erase(relevantStartStops.begin() +
                               (point), relevantStartStops.begin()+(point+2));

                    // Decrement point so that nothing is skipped.
                    point -= 2;
                }
            }

            // Move to the next highestY.
            ++highestY;
        }

        // Now that relevantStartStops is up to date, make use of it.
        for(unsigned int xVal=0; xVal<relevantStartStops.size(); ++xVal)
        {
            startStop->push_back(startVal+relevantStartStops[xVal]);
            startStopRaw->push_back(startValRaw +
                                            relevantStartStops[xVal]*density*2);
        }
    }

    // Return the result.
    return(std::make_pair(startStop, startStopRaw));
}

/**
 * Creates a colour image. Optimised for using the top image.
 */
void Fovea::makeColourTop_(const CombinedFrame& combined_frame,
    const ColourClassifier& colour_classifier,
    const std::vector<int>& startStop,
                                const std::vector<const uint8_t*>& startStopRaw)
{
    // Buffer variables for speed.
    const int doubleDensity = density*2;

    // If there is nothing to look at, return.
    if(startStop.size() == 0)
        return;

    // The current point in the saliency image.
    Colour* saliencyPixel = _colour;

    // The current point in the underlying image.
    const uint8_t* rawPixel;

    // The next change point in the saliency image.
    Colour* saliencyEnd;

    // The block currently being looked at.
    unsigned int block = 0;

    // The start needs to be classified as robot.
    if(startStop[0] != 0)
    {
        saliencyEnd = _colour + startStop[0];

        // Run mark this group of pixels as robot grey.
        for(; saliencyPixel<saliencyEnd; ++saliencyPixel)
            *saliencyPixel = cBODY_PART;
    }

    // Classify the blocks.
    while(block < startStop.size()-1)
    {
        // Even blocks use standard classification.
        saliencyPixel = _colour + startStop[block];
        saliencyEnd = _colour + startStop[block+1];
        rawPixel = startStopRaw[block];
        for(; saliencyPixel < saliencyEnd; ++saliencyPixel)
        {
            // Classify the pixel.
            *saliencyPixel = colour_classifier.classifyTop(rawPixel);

            // Move to the next raw pixel.
            rawPixel += doubleDensity;
        }

        // Move to the next block.
        ++block;
        if(!(block < startStop.size()-1))
            break;

        // Odd blocks are just robot parts.
        saliencyPixel = _colour + startStop[block];
        saliencyEnd = _colour + startStop[block+1];
        for(; saliencyPixel < saliencyEnd; ++saliencyPixel)
            *saliencyPixel = cBODY_PART;

        // Move to the next block.
        ++block;
    }

    // Classify any trailing values as robot parts.
    saliencyPixel = _colour + (*(startStop.end()-1));
    saliencyEnd = _colour + bb.height() * bb.width();
    for(; saliencyPixel<saliencyEnd; ++saliencyPixel)
        *saliencyPixel = cBODY_PART;
}

/**
 * Creates a colour image. Optimised for using the top image.
 */
void Fovea::makeColourBot_(const CombinedFrame& combined_frame,
    const ColourClassifier& colour_classifier,
    const std::vector<int>& startStop,
                                const std::vector<const uint8_t*>& startStopRaw)
{
    // Buffer variables for speed.
    const int doubleDensity = density*2;

    // If there is nothing to look at, return.
    if(startStop.size() == 0)
        return;

    // The current point in the saliency image.
    Colour* saliencyPixel = _colour;

    // The current point in the underlying image.
    const uint8_t* rawPixel;

    // The next change point in the saliency image.
    Colour* saliencyEnd;

    // The block currently being looked at.
    unsigned int block = 0;

    // The start needs to be classified as robot.
    if(startStop[0] != 0)
    {
        saliencyEnd = _colour + startStop[0];

        // Run mark this group of pixels as robot grey.
        for(; saliencyPixel<saliencyEnd; ++saliencyPixel)
            *saliencyPixel = cBODY_PART;
    }

    // Classify the blocks.
    while(block < startStop.size()-1)
    {
        // Even blocks use standard classification.
        saliencyPixel = _colour + startStop[block];
        saliencyEnd = _colour + startStop[block+1];
        rawPixel = startStopRaw[block];
        for(; saliencyPixel < saliencyEnd; ++saliencyPixel)
        {
            // Classify the pixel.
            *saliencyPixel = colour_classifier.classifyBot(rawPixel);

            // Move to the next raw pixel.
            rawPixel += doubleDensity;
        }

        // Move to the next block.
        ++block;
        if(!(block < startStop.size()-1))
            break;

        // Odd blocks are just robot parts.
        saliencyPixel = _colour + startStop[block];
        saliencyEnd = _colour + startStop[block+1];
        for(; saliencyPixel < saliencyEnd; ++saliencyPixel)
            *saliencyPixel = cBODY_PART;

        // Move to the next block.
        ++block;
    }

    // Classify any trailing values as robot parts.
    saliencyPixel = _colour + (*(startStop.end()-1));
    saliencyEnd = _colour + bb.height() * bb.width();
    for(; saliencyPixel<saliencyEnd; ++saliencyPixel)
        *saliencyPixel = cBODY_PART;
}

/**
 * Creates a 3X3 gaussian kernel blurred greyscale image.
 */
void Fovea::blurGrey_()
{
    // The distance between two saliency density y values, as the raw array is
    // in the YUV422 format (YUYVYUYV...).
    const int double_density = density*2;

    // The distance between two saliency density rows.
    const int row_size = density*2*(TOP_IMAGE_COLS*top + BOT_IMAGE_COLS*(!top));

    // The width and height of the bounding box.
    const int width = bb.width();
    const int height = bb.height();

    // The number of saliency density pixels.
    const int fovea_size = width*height;

    // A storage array used to break up the calculation and reduce the number of
    // required operations.
    int grey_temp[fovea_size];

    // The current pixel in the temp image.
    int* curr_temp;

    // The current pixel in the blurred grey image.
    int* curr_pixel;

    // The current pixel in the raw image.
    const uint8_t* curr_raw;

    /*
    The blur is applied in the form of the convolutional filter below (on the
    right), broken up into two steps. First the image is convolved by the filter
    on the left, then the result is convolved by the filter in the middle.
              [1]   [1 2 1]
    [1 2 1] * [2] = [2 4 2]
              [1]   [1 2 1]
    Where parts of the filter are off the edge of the image the "same" rule is
    applied: it is assumed the pixels off the image edge will be the same as the
    ones on the edge.
    */

    // Do the column edges.
    curr_temp = grey_temp;
    curr_raw = _rawImage;
    for(int y = 0; y < height; ++y)
    {
        // Left.
        *curr_temp = 3*(*curr_raw)+*(curr_raw+double_density);

        // Right.
        curr_temp += width-1;
        curr_raw += row_size-double_density;
        *curr_temp = *(curr_raw-double_density)+3*(*curr_raw);

        // Next.
        ++curr_temp;
        curr_raw += double_density;
    }

    // Do the image core.
    curr_temp = grey_temp+1;
    curr_raw = _rawImage + double_density;
    for(int y=0; y < height; ++y)
    {
        for (int x=1; x < width-1; ++x)
        {
            *curr_temp = *(curr_raw-double_density) + 2*(*curr_raw) +
                                                     *(curr_raw+double_density);
            ++curr_temp;
            curr_raw += double_density;
        }
        curr_temp += 2;
        curr_raw += row_size - (width-2)*double_density;
    }

    // Now do the edge rows.

    // Top.
    curr_pixel = _grey;
    curr_temp = grey_temp;
    for(int x=0; x < width; ++x)
    {
        *curr_pixel = 3*(*curr_temp) + *(curr_temp+width);
        ++curr_pixel;
        ++curr_temp;
    }

    // Bottom.
    curr_pixel = _grey+fovea_size-width;
    curr_temp = grey_temp+fovea_size-width;
    for(int x=0; x < width; ++x)
    {
        *curr_pixel = *(curr_temp-width) + 3*(*curr_temp);
        ++curr_pixel;
        ++curr_temp;
    }

    // Finally complete the classification of the image core.
    curr_pixel = _grey+width;
    curr_temp = grey_temp+width;
    for(int p = 0; p < width*(height-2); ++p)
    {
        *curr_pixel = *(curr_temp-width) + 2*(*curr_temp) + *(curr_temp+width);
        ++curr_pixel;
        ++curr_temp;
    }
}

/**
 * Creates an edge image, where x values are x axis edges, y values are y axis
 * edges and positive edges are left low right high or top low bottom high.
 */
void Fovea::makeEdge_()
{
    // The width and height of the bounding box.
    const int width = bb.width();
    const int height = bb.height();

    // The current pixel in the edge image.
    Point* curr_pixel;

    // The current pixel in the raw image.
    const int* curr_grey;

    /*
    This is a Robert's cross edge detector. Filter a and b are convolved over
    the image, and the result is summed to produce the basic edge filters below.
    a = [1  0]
        [0 -1]
    b = [0 -1]
        [1  0]
    x edge = a+b = [1 -1]
                   [1 -1]
    y edge = a-b = [ 1  1]
                   [-1 -1]
    */

    // Calculate the edges.
    curr_pixel = _edge;
    curr_grey = _grey;
    for(int y=0; y < height-1; ++y)
    {
        for(int x=0; x < width-1; ++x)
        {
            // Calculate the "cross" values.
            int a = *curr_grey - *(curr_grey+1+width);
            int b = *(curr_grey+width) - *(curr_grey+1);

            // Translate them into the edge values.
            (*curr_pixel)[0] = a + b;
            (*curr_pixel)[1] = a - b;

            // Move on to the next pixel.
            ++curr_pixel;
            ++curr_grey;
        }

        // "Same" padding on the right.
        (*curr_pixel)[0] = 0;
        (*curr_pixel)[1] = 2*(*curr_grey-*(curr_grey+width));

        // Move on to the next pixel.
        ++curr_pixel;
        ++curr_grey;
    }

    // "Same" padding on the bottom.
    for(int x=0; x < width-1; ++x)
    {
        (*curr_pixel)[0] = 2*(*curr_grey-*(curr_grey+1));
        (*curr_pixel)[1] = 0;

        // Move on to the next pixel.
        ++curr_pixel;
        ++curr_grey;
    }

    // "Same" padding on the bottom right.
    (*curr_pixel)[0] = 0;
    (*curr_pixel)[1] = 0;
}

/*
 * Gets a new fovea at a different density to this fovea. This fovea will
 * handle memory management, such that the fovea lasts until next frame.
 */
Fovea* Fovea::getChildFovea(const BBox& bounding_box,
        const int density_to_raw, const bool top,
        const bool generate_fovea_colour, const bool generate_fovea_grey,
                                                 const bool generate_fovea_edge)
{
    // Create the new fovea.
    Fovea* new_fovea = new Fovea(bounding_box, density_to_raw, top,
               generate_fovea_colour, generate_fovea_grey, generate_fovea_edge);

    // Record it for memory managment.
    child_fovea_.push_back(new_fovea);

    // Generate its saliency images.
    new_fovea->generate(*combined_frame_, *colour_classifier_);

    // Return the newly generated fovea.
    return(new_fovea);
}
