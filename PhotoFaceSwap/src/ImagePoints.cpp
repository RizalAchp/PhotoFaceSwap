#include <ImagePoints.hpp>
#include <PhotoFaceSwap.hpp>
#include "dlib/image_loader/load_image.h"
#include "dlib/image_processing/frontal_face_detector.h"

void GetShapePredictor(dlib::shape_predictor &out)
{
    std::vector<uint8_t> buff(
        _binary_shape_predictor_68_face_landmarks_dat_start,
        _binary_shape_predictor_68_face_landmarks_dat_end);
    dlib::deserialize(buff) >> out;
}

inline cv::ImagePoints2f ReadPoints(const fs::path &pointsFileName)
{
    cv::ImagePoints2f points;
    std::ifstream ifs(pointsFileName);
    float x, y;
    while (ifs >> x >> y) points.push_back(cv::Point2f(x, y));
    return points;
}

inline void GetPoints(const dlib::array2d<dlib::rgb_pixel> &img,
                      dlib::frontal_face_detector &detector,
                      dlib::shape_predictor &sp,
                      std::vector<cv::Point2f> &output)
{
    auto det = detector(img);
    LOG_DEBUG("detecting {%zu} face from image", det.size());
    if (det.size() > 1)
        LOG_WARNING(
            "detecting more than 1 face, multiple face implementation is under "
            "development!");
    if (det.empty())
    {
        LOG_ERROR(
            "cannot detect face in image! make sure the source image is "
            "valid!");
        std::exit(1);
    }

    std::vector<dlib::full_object_detection> obj;
    obj.reserve(det.size());
    for (auto &rect : det) obj.push_back(sp(img, rect));
    auto num_parts = obj[0].num_parts();

    LOG_DEBUG("getting {%zu} face detection part ", num_parts);
    output.reserve(num_parts);
    for (size_t i = 0; i < num_parts; i++)
    {
        auto &o = obj[0].part(i);
        output.push_back(cv::Point2f(o.x(), o.y()));
    }
}

inline void GetConvexHullPoints(const cv::ImagePoints2f &point_src,
                                const cv::ImagePoints2f &point_target,
                                cv::ConvexHullPoints &out)
{
    std::vector<int> hull_Idx;
    convexHull(point_src, hull_Idx, false, false);
    out.CvxHull_source.reserve(hull_Idx.size());
    out.CvxHull_target.reserve(hull_Idx.size());

    for (auto &idx : hull_Idx)
    {
        out.CvxHull_source.push_back(point_src[idx]);
        out.CvxHull_target.push_back(point_target[idx]);
    }
}

void cv::get_point_image(const fs::path &path_image_src,
                         const fs::path &path_image_target,
                         cv::ConvexHullPoints &out)
{
    dlib::shape_predictor sp;
    dlib::array2d<dlib::rgb_pixel> img_src;
    dlib::array2d<dlib::rgb_pixel> img_target;

    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    GetShapePredictor(sp);
    dlib::load_image(img_src, path_image_src);
    dlib::load_image(img_target, path_image_target);

    std::vector<Point2f> point_src;
    std::vector<Point2f> point_target;
    GetPoints(img_src, detector, sp, point_src);
    GetPoints(img_target, detector, sp, point_target);
    GetConvexHullPoints(point_src, point_target, out);
}

void cv::get_point_image(const fs::path &&predictor_file,
                         const fs::path &path_image_src,
                         const fs::path &path_image_target,
                         cv::ConvexHullPoints &out)
{
    dlib::shape_predictor sp;
    dlib::array2d<dlib::rgb_pixel> img_src;
    dlib::array2d<dlib::rgb_pixel> img_target;

    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::deserialize(predictor_file) >> sp;
    dlib::load_image(img_src, path_image_src);
    dlib::load_image(img_target, path_image_target);

    std::vector<Point2f> point_src;
    std::vector<Point2f> point_target;
    GetPoints(img_src, detector, sp, point_src);
    GetPoints(img_target, detector, sp, point_target);
    GetConvexHullPoints(point_src, point_target, out);
}

void cv::read_point_image(const fs::path &&predictor_file,
                          const fs::path &path_point_src,
                          const fs::path &path_point_target,
                          cv::ConvexHullPoints &output)
{
    TODO(predictor_file, output);
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(predictor_file) >> sp;
    auto point_src    = ReadPoints(path_point_src);
    auto point_target = ReadPoints(path_point_target);
}

void cv::save_point_image(const fs::path &path_point,
                          const ImagePoints2f &point)
{
    std::ofstream stream(path_point);
    for (auto &p : point) stream << (long)p.x << " " << (long)p.y << std::endl;
}
