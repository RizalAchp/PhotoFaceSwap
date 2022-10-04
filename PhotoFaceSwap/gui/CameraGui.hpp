#pragma once

#include <Image.hpp>
struct Cameras
{
    cv::VideoCapture Cap;
    ImageSwap frame;
    int deviceID = 0;
    int apiID    = cv::CAP_ANY;
    int width, height;

    Cameras() = default;
    ~Cameras();

    void GetCamList(std::vector<int> &dl_out);
    bool Init(int device);
    bool Run(const mahi::gui::Vec2 &size);
};
