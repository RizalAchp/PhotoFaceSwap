#include <dlib/opencv/cv_image.h>

#include <ImagePoints.hpp>
#include <PhotoFaceSwap.hpp>

#include "dlib/image_loader/load_image.h"
#include "dlib/image_processing/frontal_face_detector.h"

static dlib::shape_predictor s_SP;
static dlib::frontal_face_detector s_Detector;

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
                      std::vector<cv::Point2f> &output)
{
    auto det = s_Detector(img);
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
    for (auto &rect : det) obj.push_back(s_SP(img, rect));
    auto num_parts = obj[0].num_parts();

    LOG_DEBUG("getting {%zu} face detection part ", num_parts);
    output.reserve(num_parts);
    for (size_t i = 0; i < num_parts; i++)
    {
        auto &o = obj[0].part(i);
        output.push_back(cv::Point2f(o.x(), o.y()));
    }
}

void cv::GetConvexHullPoints(cv::ImagePoints2f &point_src,
                             cv::ImagePoints2f &point_target)
{
    std::vector<int> hull_Idx;
    cv::ImagePoints2f temp_src;
    cv::ImagePoints2f temp_target;
    temp_src.swap(point_src);
    temp_target.swap(point_target);
    convexHull(temp_target, hull_Idx, false, false);

    point_src.clear();
    point_target.clear();
    point_src.reserve(hull_Idx.size());
    point_target.reserve(hull_Idx.size());
    for (auto &idx : hull_Idx)
    {
        point_src.push_back(temp_src[idx]);
        point_target.push_back(temp_target[idx]);
    }
}

void cv::GetPointImage(const cv::Mat &mat, cv::ImagePoints2f &out)
{
    Mat tmp;
    cv::cvtColor(mat, tmp, cv::COLOR_BGR2RGB);
    dlib::array2d<dlib::rgb_pixel> dlibFrame;
    dlib::assign_image(dlibFrame, dlib::cv_image<dlib::rgb_pixel>(tmp));

    GetPoints(dlibFrame, out);
}

void cv::GetPointImage(const fs::path &path_image_src,
                       const fs::path &path_image_target,
                       cv::ConvexHullPoints &out)
{
    dlib::array2d<dlib::rgb_pixel> img_src;
    dlib::array2d<dlib::rgb_pixel> img_target;
    dlib::load_image(img_src, path_image_src);
    dlib::load_image(img_target, path_image_target);

    GetPoints(img_src, out.CvxHull_source);
    GetPoints(img_target, out.CvxHull_target);
    GetConvexHullPoints(out.CvxHull_source, out.CvxHull_source);
}

void cv::SavePointImage(const fs::path &path_point, const ImagePoints2f &point)
{
    std::ofstream stream(path_point);
    for (auto &p : point) stream << (long)p.x << " " << (long)p.y << std::endl;
}

void cv::InitPointDectector()
{
    GetShapePredictor(s_SP);
    s_Detector = dlib::get_frontal_face_detector();
}
