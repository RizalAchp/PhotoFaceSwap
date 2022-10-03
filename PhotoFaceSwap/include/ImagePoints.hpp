#pragma once
#ifndef __IMAGE_POINTS_HEADER__
#define __IMAGE_POINTS_HEADER__
#include <opencv2/opencv.hpp>
namespace fs = std::filesystem;

namespace cv
{
    typedef std::vector<Point2f> ImagePoints2f;

    void GetPointImage(const cv::Mat &_mat, std::vector<cv::ImagePoints2f> &out, bool convert);
    void GetPointImage(const fs::path &path_image_src,
                       const fs::path &path_image_target,
                       std::vector<ImagePoints2f> &out_src, std::vector<ImagePoints2f> &out_target);

    void SavePointImage(const fs::path &path_point, const ImagePoints2f &point);
    void SavePointImage(const std::vector<fs::path> &path_point,
                        const std::vector<ImagePoints2f> &point);

    void GetConvexHullPoints(cv::ImagePoints2f &point_src,
                             cv::ImagePoints2f &point_target);

    void GetConvexHullPoints(std::vector<cv::ImagePoints2f> &point_src,
                             std::vector<cv::ImagePoints2f> &point_target, size_t idx = 0);

    void InitPointDectector();

}  // namespace cv
#endif
