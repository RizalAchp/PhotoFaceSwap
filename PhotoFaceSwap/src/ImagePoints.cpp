#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>

#include <ImagePoints.hpp>

#include "PhotoFaceSwap.hpp"

/* void TestingDoang(dlib::array2d<dlib::rgb_pixel> &img,
                  std::vector<dlib::full_object_detection> &shapes)
{
    EXEC_TIMER("just testing show the image");
    dlib::image_window win;

    win.clear_overlay();
    win.set_image(img);
    win.add_overlay(render_face_detections(shapes));

    std::cout << "Hit enter to process the next image..." << std::endl;
    std::cin.get();
} */

inline cv::ImagePoints2f ReadPoints(const fs::path &pointsFileName)
{
    EXEC_TIMER("reading points from the for the object");
    cv::ImagePoints2f points;
    std::ifstream ifs(pointsFileName);
    float x, y;
    while (ifs >> x >> y) points.push_back(cv::Point2f(x, y));
    return points;
}

inline void GetPoints(const std::string &p,
                      dlib::frontal_face_detector &detector,
                      dlib::shape_predictor &sp,
                      std::vector<cv::Point2f> &output)
{
    EXEC_TIMER("getting points");
    dlib::array2d<dlib::rgb_pixel> img;
    load_image(img, p);
    // pyramid_up(img);
    auto det = detector(img);
    std::vector<dlib::full_object_detection> obj;
    obj.reserve(det.size());
    std::for_each(det.begin(), det.end(),
                  [&](dlib::rectangle &rect) { obj.push_back(sp(img, rect)); });

    output.reserve(obj[0].num_parts());
    for (size_t i = 0; i < obj[0].num_parts(); i++)
    {
        auto &o = obj[0].part(i);
        output.push_back(cv::Point2f(o.x() - 5, o.y() - 5));
    }
}

void GetConvexHullPoints(cv::ImagePoints2f &point_src,
                         cv::ImagePoints2f &point_target,
                         cv::ConvexHullPoints &out)
{
    EXEC_TIMER("find convex hull from detected points image");

    std::vector<int> hull_Idx;
    convexHull(point_src, hull_Idx, false, false);

    for (size_t i = 0; i < hull_Idx.size(); i++)
    {
        out.CvxHull_source.push_back(point_src[hull_Idx[i]]);
        out.CvxHull_target.push_back(point_target[hull_Idx[i]]);
    }
}

void cv::get_point_image(const fs::path &&predictor_file,
                         const fs::path &path_image_src,
                         const fs::path &path_image_target,
                         cv::ConvexHullPoints &out)
{
    (void)out;
    EXEC_TIMER("getting points and convex hulll from source and target image");
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(predictor_file) >> sp;

    std::vector<Point2f> point_src;
    std::vector<Point2f> point_target;
    GetPoints(path_image_src, detector, sp, point_src);
    GetPoints(path_image_target, detector, sp, point_target);
    GetConvexHullPoints(point_src, point_target, out);
}

void cv::read_point_image(const fs::path &&predictor_file,
                          const fs::path &path_point_src,
                          const fs::path &path_point_target,
                          cv::ConvexHullPoints &output)
{
    TODO(predictor_file, output);
    EXEC_TIMER("read points from file to get the convexhull");
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(predictor_file) >> sp;
    auto point_src    = ReadPoints(path_point_src);
    auto point_target = ReadPoints(path_point_target);
}

void cv::save_point_image(const fs::path &path_point,
                          const ImagePoints2f &point)
{
    EXEC_TIMER("save points from points target");
    std::ofstream stream(path_point);
    for (auto &p : point) stream << (long)p.x << " " << (long)p.y << std::endl;
}
