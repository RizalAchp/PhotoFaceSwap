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

class Writer
{
   public:
    Writer() {}
    virtual ~Writer() = default;

#ifdef PFS_GUI
    virtual void Draw(const char *, bool *, int) = 0;
#endif
    virtual void AddLog(const std::string &msg) = 0;
};

class DefaultWriter : public Writer
{
   public:
    DefaultWriter() : Writer() {}
    ~DefaultWriter() override {}
    void AddLog(const std::string &msg) override;
};

class LoggerPhotoFaceSwap
{
   public:
    LoggerPhotoFaceSwap()  = default;
    ~LoggerPhotoFaceSwap() = default;
    void Debug(const std::string &msg);
    void Warning(const std::string &msg);
    void Error(const std::string &msg);

    std::shared_ptr<Writer> writer;
    static LogLevel Level;

   public:
    static LoggerPhotoFaceSwap &Instance()
    {
        return LoggerPhotoFaceSwap::log_instance;
    }
    template <class TWriter>
    static void Init()
    {
        static_assert(std::is_base_of<Writer, TWriter>::value == true);
        LoggerPhotoFaceSwap::log_instance        = LoggerPhotoFaceSwap();
        LoggerPhotoFaceSwap::log_instance.writer = std::make_shared<TWriter>();
        LoggerPhotoFaceSwap::log_instance.Debug("Logger Initialized\n");
    }
    static LoggerPhotoFaceSwap log_instance;
};

#endif
