#pragma once
#ifndef __LOGGER_HEADER__
#define __LOGGER_HEADER__

enum LogLevel : int
{
    Debug,
    Warning,
    Error
};

void setLogLevel(int level);
class LoggerPhotoFaceSwap
{
    explicit LoggerPhotoFaceSwap() noexcept = default;
    ~LoggerPhotoFaceSwap() noexcept         = default;

   public:
    void Debug(const char *_file, int _line, const std::string &msg);
    void Warning(const char *_file, int _line, const std::string &msg);
    void Error(const char *_file, int _line, const std::string &msg);

    static LogLevel Level;

   public:
    static LoggerPhotoFaceSwap &Instance()
    {
        static LoggerPhotoFaceSwap log;
        return log;
    }
};

#endif
