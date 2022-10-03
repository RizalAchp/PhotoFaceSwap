
#include <Logger.hpp>

#include "opencv2/opencv.hpp"

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
void LoggerPhotoFaceSwap::Debug(const char *_file, int _line,
                                const std::string &msg)
{
    if (Level != LogLevel::Debug) return;

    this->writer->AddLog(
        cv::format("[DEBUG]::[%s:%d]=>%s", _file, _line, msg.c_str()));
}
void LoggerPhotoFaceSwap::Warning(const char *_file, int _line,
                                  const std::string &msg)
{
    if (Level >= LogLevel::Warning) return;
    this->writer->AddLog(
        cv::format("[WARNING]::[%s:%d]=>%s", _file, _line, msg.c_str()));
}
void LoggerPhotoFaceSwap::Error(const char *_file, int _line,
                                const std::string &msg)
{
    this->writer->AddLog(
        cv::format("[ERROR]::[%s:%d]=>%s", _file, _line, msg.c_str()));
}
