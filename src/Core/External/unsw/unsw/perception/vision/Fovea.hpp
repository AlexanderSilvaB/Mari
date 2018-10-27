#ifndef PERCEPTION_VISION_FOVEA_H_
#define PERCEPTION_VISION_FOVEA_H_

#include "perception/vision/colour/ColourClassifierInterface.hpp"
#include "perception/vision/VisionDefinitions.hpp"
#include "types/CombinedFrame.hpp"
#include "types/Point.hpp"
#include "types/BBox.hpp"


class Fovea {

public:

    /**
     * Creates a new fovea. bb is the bounds of the fovea in fovea density
     * pixels, density is the density of the fovea and top is whether this
     * fovea is in the top or bottom image. colour, grey and edge determine
     * whether the colour, blurred grey and edge images are generated
     * respectively. If edge is true grey will be generated as it is a
     * prerequisite.
     */
    Fovea(BBox bb, int density, bool top, bool colour, bool grey, bool edge) :
        bb(bb), density(density), top(top), hasColour(colour),
        hasGrey(grey || edge), hasEdge(edge),
        _colour(colour  ? new Colour[bb.width() * bb.height()] : NULL),
        _grey  (hasGrey ? new int   [bb.width() * bb.height()] : NULL),
        _edge  (hasEdge ? new Point [bb.width() * bb.height()] : NULL),
                                                       width(bb.b[0]-bb.a[0]) {}

    /**
     * Free the _colour, _grey and _edge arrays.
     */
    ~Fovea();

    /**
     * Updates the fovea to use the data in combined_frame and colour
     * classification of colour_classifier.
     */
    void generate(const CombinedFrame& combined_frame, 
        const ColourClassifier& colour_classifier);

    Fovea& operator=(const Fovea& f) { return *this; }
    const Fovea& operator=(const Fovea& f) const { return *this; }

    /**
     * Returns a pointer to the raw pixel found at x, y within the fovea. Note
     * that the pixel pointed to will be of the form YU_V if (foveaYOffset+y)*
     * foveaWidth*foveaDensity*2 + (foveaXOffset+x)*foveaDensity*2 is even, and
     * UYV otherwise. The pointer will be to the Y value.
     */
    inline const uint8_t* getRawYUV(int x, int y) const
    {
        return(_rawImage+(bb.a[1]+y)*width*density*2 + (bb.a[0]+x)*density*2);
    }

    /**
     * Returns a pointer to the raw pixel found at linearPos, where linearPos is
     * a value equivalent to (foveaYOffset+y)*foveaWidth*foveaDensity*2 +
     * (foveaXOffset+x)*foveaDensity*2 and x and y are a 2D coordinate within
     * the fovea.
     */
    inline const uint8_t* getRawYUV(int linearPos) const
    {
        return(_rawImage+linearPos);
    }

    /**
     * Returns the colour classification of the requested pixel, relative to the
     * fovea bounds. Must be inside the fovea bounds.
     */
    inline const Colour getFoveaColour(int x, int y) const
    {
        return(_colour[x + y*width]);
    }

    /**
     * Get the fovea colour, where linearPos is y*foveaWidth+x and x and y are a
     * 2D coordinate within the fovea.
     */
    inline const Colour getFoveaColour(int linearPos) const
    {
        return(_colour[linearPos]);
    }

    /**
     * Returns the blurred grey value of the requested pixel, relative to the
     * fovea bounds. Must be inside the fovea bounds.
     */
    inline const int getFoveaGrey(int x, int y) const
    {
        return(_grey[x + y*width]);
    }

    /**
     * Get the fovea blurred grey pixel, where linearPos is y*foveaWidth+x and
     * x and y are a 2D coordinate within the fovea.
     */
    inline const int getFoveaGrey(int linearPos) const
    {
        return(_grey[linearPos]);
    }

    /**
     * Returns the edge value of the requested pixel, relative to the fovea
     * bounds. Must be inside the fovea bounds. Edge values are Points such that
     * x values are x axis edges, y values are y axis edges and positive edges
     * are left low right high or top low bottom high.
     */
    inline const Point& getFoveaEdge(int x, int y) const
    {
        return(_edge[x + y*width]);
    }

    /**
     * Returns the edge value of the requested pixel, relative to the fovea
     * bounds. Must be inside the fovea bounds. Edge values are Points such that
     * x values are x axis edges, y values are y axis edges and positive edges
     * are left low right high or top low bottom high. linearPos is
     * y*foveaWidth+x and x and y are a 2D coordinate within the fovea.
     */
    inline const Point& getFoveaEdge(int linearPos) const
    {
        return(_edge[linearPos]);
    }

    /**
     * Returns the squared magnitude of the edge vector of the given pixel,
     * relative to the fovea bounds. Must be inside the fovea bounds.
     */
    inline const int getFoveaMagnitude(int x, int y) const
    {
        int pos = x + y * bb.width();
        return((_edge[pos].x()*_edge[pos].x())+(_edge[pos].y()*_edge[pos].y()));
    }

    /**
     * Get the squared magnitude of the edge vector of the given pixel, where
     * linearPos is y*foveaWidth+x and x and y are a 2D coordinate within the
     * fovea.
     */
    inline const int getFoveaMagnitude(int linearPos) const
    {
        return((_edge[linearPos].x()*_edge[linearPos].x()) +
                                   (_edge[linearPos].y()*_edge[linearPos].y()));
    }

    /**
     * Convert image coord to fovea coord. Coordinates are such that the bottom
     * image's top row is at y = TOP_IMAGE_ROWS.
     */
    inline Point mapImageToFovea(Point p) const
    {
        if (!top) p.y() -= TOP_IMAGE_ROWS;
        return (p / density) - bb.a;
    }

    /**
     * Convert fovea coord to image coord. Coordinates are such that the bottom
     * image's top row is at y = TOP_IMAGE_ROWS.
     */
    inline Point mapFoveaToImage(Point p) const
    {
        if (!top) {
            Point temp = (p + bb.a) * density;
            temp.y() += TOP_IMAGE_ROWS;
            return temp;
        }
        return (p + bb.a) * density;
    }

    /**
     * Gets the colour array for blackboard
     */
     const Colour* getInternalColour() const {
         return _colour;
     }

    /**
     * Gets BBox for fieldLineDetection
     * Gets top for fieldLineDetection
     */

     const BBox getBBox() const { return bb; }
     const bool getTop() const { return top; }
     const int getDensity() const {return density; }

     /*
      * Gets a new fovea at a different density to this fovea. This fovea will
      * handle memory management, such that the fovea lasts until next frame.
      */
     Fovea* getChildFovea(const BBox& bounding_box,
         const int density_to_raw, const bool top,
         const bool generate_fovea_colour, const bool generate_fovea_grey,
                                                const bool generate_fovea_edge);

protected:
    /**
     * Gets a pointer to the first item in the colour array.
     * Temporary, for future speed comparison with iterator.
     */
    const Colour* getColourArray();

    /**
     * Gets a pointer to the first item in the raw image.
     * Temporary, for future speed comparison with iterator.
     */
    const uint8_t* getRawPixelArray();

private:

    // The bounds of the fovea in fovea density pixels.
    const BBox           bb;

    // The number of raw image pixels per classified pixel on each axis for this
    // fovea.
    const int            density;

    // Whether this fovea is in the top image.
    const bool           top;

    // Whether this fovea has a colour image.
    const bool           hasColour;

    // Whether this fovea has a grey image.
    const bool           hasGrey;

    // Whether this fovea has an edge image.
    const bool           hasEdge;

    // The colour classified image.
     Colour      *const _colour;

    // The blurred greyscale image.
    int          *const _grey;

    // The edge image, where x values are x axis edges, y values are y axis
    // edges and positive edges are left low right high or top low bottom high.
    Point        *const _edge;

    // The width of the fovea in density pixels.
    const int            width;

    // A pointer to the upper left pixel of the image this fovea is in.
    const uint8_t *     _rawImage;

    // These need to be available for generating child foveas.
    const CombinedFrame* combined_frame_;
    const ColourClassifier* colour_classifier_;

    // All this fovea's child fovea.
    std::vector<Fovea*> child_fovea_;

    /**
     * Creates a colour image. Optimised for using the top image.
     */
    void makeColourTop_(const CombinedFrame& combined_frame,
        const ColourClassifier& colour_classifier,
        const std::vector<int>& startStop,
                               const std::vector<const uint8_t*>& startStopRaw);

    /**
     * Creates a colour image. Optimised for using the bottom image.
     */
    void makeColourBot_(const CombinedFrame& combined_frame,
        const ColourClassifier& colour_classifier,
        const std::vector<int>& startStop,
                               const std::vector<const uint8_t*>& startStopRaw);

    /**
     * Creates a 3X3 gaussian kernel blurred greyscale image.
     */
    void blurGrey_();

    /**
     * Creates an edge image, where x values are x axis edges, y values are y axis
     * edges and positive edges are left low right high or top low bottom high.
     */
    void makeEdge_();

    /**
     * Returns the raw grey value of the requested pixel, relative to the fovea
     * bounds. Must be inside the fovea bounds.
     */
    inline const uint8_t& getRawGrey_(int x, int y) const
    {
        static int imageCols;
        imageCols = top? TOP_IMAGE_COLS : BOT_IMAGE_COLS;
        return (_rawImage[x*density*2 + y*imageCols*density*2]);
    }

    /*
     * Translates the y axis robot part stop array to a linear start stop array.
     */
    std::pair<std::vector<int>*, std::vector<const uint8_t*>* > getStartStop_()
                                                                          const;
};

#endif
