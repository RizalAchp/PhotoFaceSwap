#pragma once
#ifndef __IMAGE_POINTS_HEADER__
#define __IMAGE_POINTS_HEADER__
#include <opencv2/opencv.hpp>
namespace fs = std::filesystem;

namespace cv
{
    typedef std::vector<Point2f> ImagePoints2f;
    struct ConvexHullPoints
    {
        ImagePoints2f CvxHull_source;
        ImagePoints2f CvxHull_target;

        ConvexHullPoints(const ImagePoints2f &pointsrc,
                         const ImagePoints2f &pointtarget)
            : CvxHull_source(pointsrc), CvxHull_target(pointtarget)
        {
        }

        ConvexHullPoints() = default;
    };

    void GetPointImage(const cv::Mat &mat, cv::ImagePoints2f &out);
    void GetPointImage(const fs::path &path_image_src,
                       const fs::path &path_image_target,
                       cv::ConvexHullPoints &out);

    void SavePointImage(const fs::path &path_point, const ImagePoints2f &point);
    void SavePointImage(const std::vector<fs::path> &path_point,
                        const std::vector<ImagePoints2f> &point);

    void GetConvexHullPoints(cv::ImagePoints2f &point_src,
                             cv::ImagePoints2f &point_target);

    void InitPointDectector();

}  // namespace cv
#endif
