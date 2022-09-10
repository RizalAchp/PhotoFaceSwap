#pragma once
#ifndef __LOGGER_HEADER__
#define __LOGGER_HEADER__

enum LogLevel : int
{
    Debug,
    Warning,
    Error
};

class Logger
{
    explicit Logger() noexcept = default;
    ~Logger() noexcept         = default;

   public:
    void Debug(const char *_file, int _line, const std::string &msg);
    void Warning(const char *_file, int _line, const std::string &msg);
    void Error(const char *_file, int _line, const std::string &msg);

    void Init(LogLevel level);

   private:
    LogLevel m_Level = LogLevel::Warning;

   public:
    static Logger &Instance()
    {
        static Logger log;
        return log;
    }
};

#endif
