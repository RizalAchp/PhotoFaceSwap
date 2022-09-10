#pragma once

#ifndef __PHOTO_FACE_SWAP_HEADER__
#define __PHOTO_FACE_SWAP_HEADER__

#include <ArgumentParse.hpp>
#include <ImagePoints.hpp>
#include <Logger.hpp>
#include <opencv2/opencv.hpp>

#define LOG_DEBUG(...) \
    Logger::Instance().Debug(__FILE_NAME__, __LINE__, cv::format(__VA_ARGS__))
#define LOG_WARNING(...) \
    Logger::Instance().Warning(__FILE_NAME__, __LINE__, cv::format(__VA_ARGS__))
#define LOG_ERROR(...) \
    Logger::Instance().Error(__FILE_NAME__, __LINE__, cv::format(__VA_ARGS__))

int MainCli(argparse::ArgumentParser &args);
int MainGui(argparse::ArgumentParser &args);

namespace cv
{
    void ProcessImage(const fs::path &_src, const fs::path &_target,
                      const cv::ConvexHullPoints &points, cv::Mat &output);

    void ShowResult(cv::Mat &result);

    void SaveResult(std::string &&folder, cv::Mat &result);
}  // namespace cv

class ExecutionTimer
{
   public:
    using Resolution = std::chrono::microseconds;
    using Clock =
        std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>;

   private:
    const char *m_Desc;
    const char *m_Filename;
    const int m_Line;
    const char *m_Func;
    Clock::time_point mStart;

   public:
    ExecutionTimer(const char *filename, const int line, const char *func,
                   const char *desc)
        : m_Desc(desc)
        , m_Filename(filename)
        , m_Line(line)
        , m_Func(func)
        , mStart(Clock::now())
    {
        using namespace termcolor;
        std::cerr << bright_blue << underline << "[TIMER START]" << reset
                  << blue << "[" << m_Filename << ":" << m_Line << "]::["
                  << m_Func << "] => " << reset << bold << m_Desc << std::endl;
    }

    ~ExecutionTimer()
    {
        using namespace termcolor;
        auto end = Clock::now() - mStart;
        std::cerr << bright_blue << underline << "[TIMER STOP]" << reset << blue
                  << "[" << m_Func << "] => " << reset << bold << "elapsed in: "
                  << std::chrono::duration<double>(end).count() << " second | "
                  << std::chrono::duration_cast<Resolution>(end).count()
                  << " us" << std::endl;
    }
    operator bool() { return true; }
};

#define EXEC_TIMER(_DESC) \
    ExecutionTimer T(__FILE_NAME__, __LINE__, __FUNCTION__, _DESC)
#define EXEC_TIMER_MS(_DESC) \
    ExecutionTimer T(__FILE_NAME__, __LINE__, __FUNCTION__, _DESC)

#endif
