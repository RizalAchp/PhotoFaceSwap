#pragma once

#ifndef __PHOTO_FACE_SWAP_HEADER__
#define __PHOTO_FACE_SWAP_HEADER__
extern const unsigned char
    _binary_shape_predictor_68_face_landmarks_dat_start[];
extern const unsigned char _binary_shape_predictor_68_face_landmarks_dat_size;
extern const unsigned char _binary_shape_predictor_68_face_landmarks_dat_end[];

#include <dlib/image_processing.h>

#include <ArgumentParse.hpp>
#include <ImagePoints.hpp>
#include <Logger.hpp>
#include <opencv2/opencv.hpp>

#define LOG_DEBUG(...)                                             \
    LoggerPhotoFaceSwap::Instance().Debug(__FILE_NAME__, __LINE__, \
                                          cv::format(__VA_ARGS__))
#define LOG_WARNING(...)                                             \
    LoggerPhotoFaceSwap::Instance().Warning(__FILE_NAME__, __LINE__, \
                                            cv::format(__VA_ARGS__))
#define LOG_ERROR(...)                                             \
    LoggerPhotoFaceSwap::Instance().Error(__FILE_NAME__, __LINE__, \
                                          cv::format(__VA_ARGS__))

int MainCli(argparse::ArgumentParser &args);

void GetShapePredictor(dlib::shape_predictor &out);
namespace cv
{
    void ProcessImage(const fs::path &_src, const fs::path &_target,
                      const cv::ConvexHullPoints &points, cv::Mat &output);

    void ShowResult(cv::Mat &result);

    void SaveResult(std::string &&folder, cv::Mat &result);
}  // namespace cv

#endif
