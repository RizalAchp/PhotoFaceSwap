#include <CameraGui.hpp>

Cameras::~Cameras()
{
    this->frame.Close();
    this->Cap.release();
}

bool Cameras::Init(int device)
{
    this->deviceID = device;
    this->Cap      = cv::VideoCapture(this->deviceID);
    return this->Cap.isOpened();
}

bool Cameras::Run(const mahi::gui::Vec2 &size)
{
    if (!this->Cap.isOpened()) return false;
    this->Cap >> this->frame.raw;
    if (!this->Cap.read(this->frame.raw)) return false;
    this->frame.Resize(size);
    this->frame.GetPointSelected();
    this->frame.Draw();
    return true;
}

void Cameras::GetCamList(std::vector<int> &dl_out)
{
    for (int i = 0; i < 10; i++)
    {
        try
        {
            auto cap = cv::VideoCapture(i);
            if (!cap.isOpened())
            {
                cap.release();
                continue;
            }
            cv::Mat mat;
            cap.read(mat);
            dl_out.push_back(i);
        }
        catch (const cv::Exception &e)
        {
            continue;
        }
    }
}
