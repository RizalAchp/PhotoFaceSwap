#pragma once
#ifndef __CALCULATE_HEADER__
#define __CALCULATE_HEADER__

namespace cv
{
    void applyAffineTransform(Mat &warpImage, Mat &src, ImagePoints2f &srcTri,
                              ImagePoints2f &dstTri);

    void calculateDelaunayTriangles(Rect rect, ImagePoints2f &points,
                                    std::vector<std::vector<int>> &delaunayTri);

    void warpTriangle(Mat &img1, Mat &img2, ImagePoints2f &t1, ImagePoints2f &t2);
}  // namespace cv
#endif
