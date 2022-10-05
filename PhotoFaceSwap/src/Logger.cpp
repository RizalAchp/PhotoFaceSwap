
#include <Logger.hpp>

#include "opencv2/opencv.hpp"
inline static void TimeNow(std::ostream &is)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    is << std::put_time(&tm, "%H:%M:%S");
}

void setLogLevel(int level)
{
    switch (level)
    {
        case LogLevel::Debug:
            LoggerPhotoFaceSwap::Level = LogLevel::Debug;
            break;
        case LogLevel::Warning:
            LoggerPhotoFaceSwap::Level = LogLevel::Warning;
            break;
        case LogLevel::Error:
            LoggerPhotoFaceSwap::Level = LogLevel::Error;
            break;
        default:
            break;
    }
}

void DefaultWriter::AddLog(const std::string &msg) { std::cerr << msg; }

LoggerPhotoFaceSwap LoggerPhotoFaceSwap::log_instance;
LogLevel LoggerPhotoFaceSwap::Level = LogLevel::Error;

void LoggerPhotoFaceSwap::Debug(const std::string &msg)
{
    if (Level != LogLevel::Debug) return;
    std::stringstream time;
    TimeNow(time);
    this->writer->AddLog(cv::format("[DEBUG]::[%s]=>%s", time.str(), msg.c_str()));
}
void LoggerPhotoFaceSwap::Warning(const std::string &msg)
{
    if (Level >= LogLevel::Warning) return;
    std::stringstream time;
    TimeNow(time);
    this->writer->AddLog(
        cv::format("[WARNING]::[%s:%d]=>%s", time.str(), msg.c_str()));
}
void LoggerPhotoFaceSwap::Error(const std::string &msg)
{
    std::stringstream time;
    TimeNow(time);

    this->writer->AddLog(
        cv::format("[ERROR]::[%s:%d]=>%s", time.str(), msg.c_str()));
}
