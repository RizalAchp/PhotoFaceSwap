#pragma once
#include <tuple>
#ifndef __IMAGE_POINTS_HEADER__
#define __IMAGE_POINTS_HEADER__
#include <cassert>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#define UNUSED_PARAM(P) ((void)P)

#define TODO(param1, param2)                                                                \
    UNUSED_PARAM(param1);                                                                   \
    UNUSED_PARAM(param2);                                                                   \
    std::cout << "TODO: " << __FILE__ << __LINE__ << " on " << __PRETTY_FUNCTION__ << "\n"; \
    throw std::runtime_error("TODO!");

namespace fs = std::filesystem;

namespace cv
{
    struct ConvexHullPoints
    {
        std::vector<Point2f> CvxHull_source;
        std::vector<Point2f> CvxHull_target;
        std::vector<int> HullIndex;
    };

    ConvexHullPoints get_point_image(const fs::path &path_image_src,
                                     const fs::path &path_image_target);

    ConvexHullPoints read_point_image(const fs::path &path_point_src,
                                     const fs::path &path_point_target);

    std::vector<ConvexHullPoints> get_point_multi(const std::vector<fs::path> &path_image_src,
                                                  const std::vector<fs::path> &path_image_target);

    std::vector<ConvexHullPoints> read_point_multi(const std::vector<fs::path> &path_point_src,
                                                  const std::vector<fs::path> &path_point_target);

    std::vector<ConvexHullPoints> get_point_multi_target(
        const fs::path &path_image_src, const std::vector<fs::path> &path_image_target);

    std::vector<ConvexHullPoints> read_point_multi_target(
        const fs::path &path_image_src, const std::vector<fs::path> &path_image_target);

}  // namespace cv
#endif
