
#include <ImagePoints.hpp>

cv::ConvexHullPoints get_point_image(const fs::path &path_image_src,
                                     const fs::path &path_image_target)
{
    TODO(path_image_src, path_image_target);
}

cv::ConvexHullPoints read_point_image(const fs::path &path_point_src,
                                      const fs::path &path_point_target)
{
    TODO(path_point_src, path_point_target);
}

std::vector<cv::ConvexHullPoints> get_point_multi(const std::vector<fs::path> &path_image_src,
                                                  const std::vector<fs::path> &path_image_target)
{
    TODO(path_image_src, path_image_target);
}

std::vector<cv::ConvexHullPoints> read_point_multi(const std::vector<fs::path> &path_point_src,
                                                   const std::vector<fs::path> &path_point_target)
{
    TODO(path_point_src, path_point_target);
}

std::vector<cv::ConvexHullPoints> get_point_multi_target(
    const fs::path &path_image_src, const std::vector<fs::path> &path_image_target)
{
    TODO(path_image_src, path_image_target);
}

std::vector<cv::ConvexHullPoints> read_point_multi_target(
    const fs::path &path_point_src, const std::vector<fs::path> &path_point_target)
{
    TODO(path_point_src, path_point_target);
}
