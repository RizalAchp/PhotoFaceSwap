#pragma once

#ifndef __PHOTO_FACE_SWAP_HEADER__
#define __PHOTO_FACE_SWAP_HEADER__
extern const unsigned char
    _binary_shape_predictor_68_face_landmarks_dat_start[];
extern const unsigned char _binary_shape_predictor_68_face_landmarks_dat_size;
extern const unsigned char _binary_shape_predictor_68_face_landmarks_dat_end[];

#ifndef PFS_GUI
#include <ArgumentParse.hpp>
#endif
#include <Logger.hpp>
namespace fs = std::filesystem;

#define LOG_DEBUG(...)                                             \
    LoggerPhotoFaceSwap::Instance().Debug(__FILE_NAME__, __LINE__, \
                                          cv::format(__VA_ARGS__))
#define LOG_WARNING(...)                                             \
    LoggerPhotoFaceSwap::Instance().Warning(__FILE_NAME__, __LINE__, \
                                            cv::format(__VA_ARGS__))
#define LOG_ERROR(...)                                             \
    LoggerPhotoFaceSwap::Instance().Error(__FILE_NAME__, __LINE__, \
                                          cv::format(__VA_ARGS__))
#ifdef PFS_GUI
#define LOG_DRAW(...) LoggerPhotoFaceSwap::Instance().writer->Draw(__VA_ARGS__)
#else
int MainCli(argparse::ArgumentParser &args);
#endif

namespace cv
{

    typedef std::vector<Point2f> ImagePoints2f;

    void GetShapePredictor(dlib::shape_predictor &out);
    void GetPointImage(const cv::Mat &_mat, std::vector<cv::ImagePoints2f> &out,
                       bool convert);
    void GetPointImage(const fs::path &path_image_src,
                       const fs::path &path_image_target,
                       std::vector<ImagePoints2f> &out_src,
                       std::vector<ImagePoints2f> &out_target);

    void SavePointImage(const fs::path &path_point, const ImagePoints2f &point);
    void SavePointImage(const std::vector<fs::path> &path_point,
                        const std::vector<ImagePoints2f> &point);

    void GetConvexHullPoints(ImagePoints2f &point_src,
                             ImagePoints2f &point_target);

    void GetConvexHullPoints(std::vector<ImagePoints2f> &point_src,
                             std::vector<ImagePoints2f> &point_target,
                             size_t idx = 0);

    void InitPointDectector();

    void ProcessImage(const Mat &_src_mat, const Mat &_tgt_mat,
                      const ImagePoints2f &poinsrc,
                      const ImagePoints2f &pointarget, cv::Mat &output);

    inline void SaveResult(const fs::path &file, const cv::Mat &result)
    {
        imwrite(file, result);
    }
}  // namespace cv

#endif
