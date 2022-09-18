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
    };

    void get_point_image(const fs::path &path_image_src,
                         const fs::path &path_image_target,
                         cv::ConvexHullPoints &out);
    void get_point_image(const fs::path &&predictor_file,
                         const fs::path &path_image_src,
                         const fs::path &path_image_target,
                         ConvexHullPoints &output);

    void read_point_image(const fs::path &&predictor_file,
                          const fs::path &path_point_src,
                          const fs::path &path_point_target,
                          ConvexHullPoints &output);

    void save_point_image(const fs::path &path_point,
                          const ImagePoints2f &point);
    void save_point_image(const std::vector<fs::path> &path_point,
                          const std::vector<ImagePoints2f> &point);

}  // namespace cv
#endif
