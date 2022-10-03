#include <dlib/image_loader/load_image.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv/cv_image.h>

#include <PhotoFaceSwap.hpp>
#include <opencv2/imgproc.hpp>

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
                      std::vector<cv::ImagePoints2f> &output)
{
    std::vector<dlib::rectangle> det = s_Detector(img);
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
        return;
    }

    output.reserve(det.size());
    for (auto &rect : det)
    {
        auto fod = s_SP(img, rect);
        auto np  = fod.num_parts();
        cv::ImagePoints2f ip;
        ip.reserve(np);

        for (size_t i = 0; i < np; i++)
        {
            auto &pt = fod.part(i);
            ip.push_back(cv::Point2f(pt.x(), pt.y()));
        }

        output.push_back(ip);
    }
}

void cv::GetConvexHullPoints(cv::ImagePoints2f &point_src,
                             cv::ImagePoints2f &point_target)
{
    std::vector<int> hull_Idx;
    cv::ImagePoints2f temp_src(point_src);
    cv::ImagePoints2f temp_target(point_target);
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

void cv::GetConvexHullPoints(std::vector<cv::ImagePoints2f> &point_src,
                             std::vector<cv::ImagePoints2f> &point_target,
                             size_t idx)
{
    int sz_src = point_src.size();
    std::vector<ImagePoints2f> tmp_target(sz_src, point_target[idx]);
    for (int i = 0; i < sz_src; i++)
    {
        GetConvexHullPoints(point_src[i], tmp_target[i]);
    }

    point_target.clear();
    point_target.swap(tmp_target);
}

void cv::GetPointImage(const cv::Mat &_mat, std::vector<cv::ImagePoints2f> &out,
                       bool convert)
{
    cv::Mat mat;
    _mat.copyTo(mat);
    if (convert)
    {
        cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
    }
    dlib::array2d<dlib::rgb_pixel> dlibFrame;
    dlib::assign_image(dlibFrame, dlib::cv_image<dlib::rgb_pixel>(mat));

    GetPoints(dlibFrame, out);
}

void cv::GetPointImage(const fs::path &path_image_src,
                       const fs::path &path_image_target,
                       std::vector<ImagePoints2f> &out_src,
                       std::vector<ImagePoints2f> &out_target)
{
    dlib::array2d<dlib::rgb_pixel> img_src;
    dlib::array2d<dlib::rgb_pixel> img_target;
    dlib::load_image(img_src, path_image_src);
    dlib::load_image(img_target, path_image_target);

    GetPoints(img_src, out_src);
    GetPoints(img_target, out_target);
    GetConvexHullPoints(out_src, out_target);
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
