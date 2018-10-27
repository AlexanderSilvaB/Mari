/*
Copyright 2010 The University of New South Wales (UNSW).

This file is part of the 2010 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef PERCEPTION_VISION_MIDDLEINFOPROCESSOR_FIELD_BOUNDARY_FINDER
#define PERCEPTION_VISION_MIDDLEINFOPROCESSOR_FIELD_BOUNDARY_FINDER

#include <vector>
#include <utility>

#include "perception/vision/VisionDefinitions.hpp"
#include "perception/vision/camera/CameraToRR.hpp"
#include "perception/vision/Fovea.hpp"

#include "types/VisionInfoIn.hpp"
#include "types/VisionInfoOut.hpp"
#include "types/FieldBoundaryInfo.hpp"
#include "types/Point.hpp"

#include "MiddleInfoProcessorInterface.hpp"

class FieldBoundaryFinder : public MiddleInfoProcessor
{
   public:
      /**
       * Points to be ransaced
      **/
      std::vector<Point> boundaryPointsTop;
      std::vector<Point> boundaryPointsBot;

      /**
       * The coordinates of the top of the field in the image
       * The coordinates are given in image coordinates
       **/
      int topStartScanCoords[IMAGE_COLS];
      int botStartScanCoords[IMAGE_COLS];

      void find(const VisionInfoIn& info_in, VisionInfoMiddle& info_middle, VisionInfoOut& info_out);

      /**
       * Find coordinates of points that may be at the boundary
       * of the field by using the saliency scan
       * @param frame      Current vision frame
       * @param fovea      Current fovea to be searched
       **/
      void fieldBoundaryPoints(const VisionInfoIn &info_in,
                           const Fovea &fovea,
                           bool top);

      /**
       * Find up to two lines formed by field boundary points
       * using the RANSAC algorithm
       **/
      void fieldBoundaryLines(unsigned int *seed,
                          const CameraToRR *convRR,
                          bool top);

      /**
       * Fills the startScanCoords array to find the coordinates
       * in the saliency scan where the field starts
       **/
      void findStartScanCoords(const VisionInfoIn &info_in,
                               VisionInfoOut &info_out,
                               const Fovea &fovea);

      explicit FieldBoundaryFinder();

      std::vector<FieldBoundaryInfo> fieldBoundaries;

   private:
      static const int consecutive_green;

      void lsRefineLine(
            RANSACLine               &line,
            const std::vector<Point> &points,
            const std::vector<bool>  &cons);

      /**
       * A cummulative count of green pixels occuring
       * at the top of the image
       */
      int greenTops[TOP_SALIENCY_COLS];
      int totalGreens;
};

#endif
