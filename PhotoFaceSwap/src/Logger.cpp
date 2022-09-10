#include <sys/ioctl.h>

#include <Logger.hpp>
#include <TermColor.hpp>

void Logger::Init(LogLevel level) { m_Level = level; }

void Logger::Debug(const char *_file, int _line, const std::string &msg)
{
    if (m_Level != LogLevel::Debug) return;
    std::cerr << termcolor::green << termcolor::underline << "[DEBUG]"
              << termcolor::reset << termcolor::bright_green << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
void Logger::Warning(const char *_file, int _line, const std::string &msg)
{
    if (m_Level > LogLevel::Warning) return;
    std::cerr << termcolor::yellow << termcolor::underline << "[WARNING]"
              << termcolor::reset << termcolor::bright_yellow << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
void Logger::Error(const char *_file, int _line, const std::string &msg)
{
    std::cerr << termcolor::red << termcolor::underline << "[ERROR]"
              << termcolor::reset << termcolor::bright_red << "::[" << _file
              << ":" << _line << "]" << termcolor::reset << " => "
              << termcolor::underline << std::right << msg << termcolor::reset
              << std::endl;
}
