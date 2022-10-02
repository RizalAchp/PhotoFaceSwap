#pragma once
#include <GuiLogger.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Gui/Fonts.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Util.hpp>
#include <PhotoFaceSwap.hpp>
#include <mutex>
#include <thread>

#include "ImagePoints.hpp"

enum TypeImage : int
{
    Source,
    Target,
    Output,
};

struct ImageProcessing
{
    ImageProcessing() = default;

    GLuint texture_source;
    GLuint texture_target;
    GLuint texture_output;
    cv::Mat source;
    cv::Mat target;
    cv::Mat output;
    cv::ImagePoints2f p_source;
    cv::ImagePoints2f p_target;

    void OpenImage(TypeImage type, const std::string &file, float height);
    void ResizeImage(TypeImage type, float height);
    void ProcessPoints(TypeImage type);
    void DrawImage(TypeImage type);
    void UpdateTexture(TypeImage type);
};
