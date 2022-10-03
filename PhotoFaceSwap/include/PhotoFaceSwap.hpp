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
#endif

int MainCli(argparse::ArgumentParser &args);

void GetShapePredictor(dlib::shape_predictor &out);
namespace cv
{

    void ProcessImage(const Mat &_src_mat, const Mat &_tgt_mat,
                      const cv::ImagePoints2f &poinsrc,
                      const cv::ImagePoints2f &pointarget, cv::Mat &output);

    void SaveResult(std::string &&folder, cv::Mat &result);
}  // namespace cv

#endif
