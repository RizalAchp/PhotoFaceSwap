#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>

#include <ImagePoints.hpp>

inline cv::ImagePoints2f GetOuterPath(const dlib::full_object_detection &&shape)
{
    cv::ImagePoints2f temp_p;
    for (size_t i = 0; i < shape.num_parts(); i++)
        temp_p.push_back(cv::Point2f((float)shape.part(i).x(), (float)shape.part(i).y()));
    return temp_p;
}

inline cv::ImagePoints2f ReadPoints(const fs::path &pointsFileName)
{
    cv::ImagePoints2f points;
    std::ifstream ifs(pointsFileName);
    float x, y;
    while (ifs >> x >> y) points.push_back(cv::Point2f(x, y));
    return points;
}

inline cv::ImagePoints2f GetPointSrc(const fs::path &p, dlib::frontal_face_detector &detector,
                                     dlib::shape_predictor &sp)
{
    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, p);
    dlib::pyramid_up(img);

    std::vector<dlib::rectangle> dets = detector(img);
    if (dets.size() > 1)
    {
        std::cout << "WARNING: source image: " << p.string()
                  << " detecting more than one face!\n"
                     "program will ignoring other face, and using the first detected image"
                  << std::endl;
    }
    return GetOuterPath(sp(img, dets.at(0)));
}

inline std::vector<cv::ImagePoints2f> GetPointTarget(const fs::path &p,
                                                     dlib::frontal_face_detector &detector,
                                                     dlib::shape_predictor &sp)
{
    std::vector<std::vector<cv::Point2f>> out;

    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, p);
    dlib::pyramid_up(img);

    std::vector<dlib::rectangle> dets = detector(img);
    for (dlib::rectangle &det : dets) out.push_back(GetOuterPath(sp(img, det)));

    return out;
}

inline cv::ConvexHullPoints GetConvexHullPoint(cv::ImagePoints2f &&point_src,
                                               cv::ImagePoints2f &&point_target)
{
    cv::ConvexHullPoints out;
    // Find convex hull
    convexHull(point_target, out.HullIndex, false, false);

    cv::ImagePoints2f temp_target;
    temp_target.reserve(out.HullIndex.size());
    out.CvxHull_source.reserve(out.HullIndex.size());

    for (auto &idx : out.HullIndex)
    {
        out.CvxHull_source.push_back(point_src[static_cast<size_t>(idx)]);
        temp_target.push_back(point_target[static_cast<size_t>(idx)]);
    }
    out.CvxHull_target.push_back(std::move(temp_target));
    return out;
}

inline cv::ConvexHullPoints GetConvexHullPoint(cv::ImagePoints2f &&point_src,
                                               std::vector<cv::ImagePoints2f> &&point_target)
{
    cv::ConvexHullPoints out;
    for (auto &target : point_target)
    {
        // Find convex hull
        convexHull(target, out.HullIndex, false, false);

        cv::ImagePoints2f temp_target;
        temp_target.reserve(out.HullIndex.size());
        out.CvxHull_source.reserve(out.HullIndex.size());

        for (auto &idx : out.HullIndex)
        {
            out.CvxHull_source.push_back(point_src[static_cast<size_t>(idx)]);
            temp_target.push_back(target[static_cast<size_t>(idx)]);
        }
        out.CvxHull_target.push_back(std::move(temp_target));
    }
    return out;
}

cv::ConvexHullPoints cv::get_point_image(const fs::path &path_image_src,
                                         const fs::path &path_image_target)
{
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    /// TODO!
    dlib::deserialize("predictor") >> sp;
    auto point_src    = GetPointSrc(path_image_src, detector, sp);
    auto point_target = GetPointTarget(path_image_target, detector, sp);

    return GetConvexHullPoint(std::move(point_src), std::move(point_target));
}

cv::ConvexHullPoints cv::read_point_image(const fs::path &path_point_src,
                                          const fs::path &path_point_target)
{
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    /// TODO!
    dlib::deserialize("predictor") >> sp;
    auto point_src    = ReadPoints(path_point_src);
    auto point_target = ReadPoints(path_point_target);
    return GetConvexHullPoint(std::move(point_src), std::move(point_target));
}

void cv::save_point_image(const fs::path &path_point, const ImagePoints2f &point)
{
    std::ofstream stream(path_point);
    for (auto &p : point) stream << (long)p.x << " " << (long)p.y << std::endl;
}
void cv::save_point_image(const std::vector<fs::path> &path_point,
                          const std::vector<ImagePoints2f> &point)
{
    assert(path_point.size() == point.size());
    for (size_t i = 0; i < path_point.size(); i++)
    {
        save_point_image(path_point[i], point[i]);
    }
}
