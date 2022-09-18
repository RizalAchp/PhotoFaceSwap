#include <sys/ioctl.h>

#include <Logger.hpp>
#include <TermColor.hpp>

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

LogLevel LoggerPhotoFaceSwap::Level = LogLevel::Error;

void LoggerPhotoFaceSwap::Debug(const char *_file, int _line,
                                const std::string &msg)
{
    if (Level != LogLevel::Debug) return;
    std::cerr << termcolor::green << termcolor::underline << "[DEBUG]"
              << termcolor::reset << termcolor::bright_green << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
void LoggerPhotoFaceSwap::Warning(const char *_file, int _line,
                                  const std::string &msg)
{
    if (Level > LogLevel::Warning) return;
    std::cerr << termcolor::yellow << termcolor::underline << "[WARNING]"
              << termcolor::reset << termcolor::bright_yellow << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
void LoggerPhotoFaceSwap::Error(const char *_file, int _line,
                                const std::string &msg)
{
    std::cerr << termcolor::red << termcolor::underline << "[ERROR]"
              << termcolor::reset << termcolor::bright_red << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
