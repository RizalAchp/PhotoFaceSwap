#include <PhotoFaceSwap.hpp>

#include "dlib/gui_widgets/widgets.h"

void setLogLevel(int level)
{
    switch (level)
    {
        case LogLevel::Debug:
            Logger::Instance().Init(LogLevel::Debug);
            break;
        case LogLevel::Warning:
            Logger::Instance().Init(LogLevel::Warning);
            break;
        case LogLevel::Error:
            Logger::Instance().Init(LogLevel::Error);
            break;
        default:
            Logger::Instance().Init(LogLevel::Error);
            break;
    }
}

int MainCli(argparse::ArgumentParser &args)
{
    using namespace std;
    using namespace cv;
    LOG_DEBUG("STARTING CLI Mode");
    EXEC_TIMER_MS("Time Starting Program in CLI MODE");

    setLogLevel(args.get<int>("-d"));
    fs::path file_source = args.get("source_img");
    if (!fs::exists(file_source))
    {
        LOG_ERROR(
            "source file %s is not exists!"
            "\nexiting program..",
            file_source.c_str());
        return 1;
    }
    fs::path file_target = args.get("target_img");
    if (!fs::exists(file_target))
    {
        LOG_ERROR(
            "target file %s is not exists!"
            "\nexiting program..",
            file_target.c_str());
        return 1;
    }
    ConvexHullPoints ch_points;
    get_point_image(args.get("predictor"), file_source, file_target, ch_points);

    cv::Mat result;
    ProcessImage(file_source, file_target, ch_points, result);
    ShowResult(result);

    fs::path file_out = args.get("-o");
    file_out.replace_extension(file_target.extension());
    cv::imwrite(file_out, result);
    return 0;
}
