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

enum FlipCode : int
{
    Vertical   = -1,
    Horizontal = 0,
};
constexpr const char *FLIP_CODE_NAME[] = {"Vertical", "Horizontal"};

struct ImageSwap
{
    ImageSwap() = default;
    ~ImageSwap();
    GLuint texture;
    cv::Mat raw;
    cv::Mat resized;
    std::vector<cv::ImagePoints2f> points;
    std::vector<cv::ImagePoints2f> points_show;
    size_t point_selected;

    bool Open(const std::string &file) noexcept(false);
    void Resize(const mahi::gui::Vec2 &size) noexcept(false);
    void Flip(int code);
    void Close() noexcept;

    void Draw() noexcept(false);
    void UpdateTexture() noexcept(false);
    bool ProcessPoints() noexcept(false);

    inline cv::ImagePoints2f &GetPointSelected() noexcept
    {
        return this->points.at(point_selected);
    }
};
