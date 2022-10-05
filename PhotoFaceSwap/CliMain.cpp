#include <PhotoFaceSwap.hpp>
#include <TermColor.hpp>

int MainCli(argparse::ArgumentParser &args)
{
    using namespace std;
    using namespace cv;
    LOG_DEBUG("STARTING CLI Mode");

    LoggerPhotoFaceSwap::Init<DefaultWriter>();
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
    cv::Mat source = cv::imread(file_source.string());
    cv::Mat target = cv::imread(file_target.string());
    std::vector<ImagePoints2f> points_source;
    std::vector<ImagePoints2f> points_target;
    GetPointImage(source, points_source, false);
    GetPointImage(target, points_target, false);

    cv::Mat result;
    ProcessImage(source, target, points_source[0], points_target[0], result);

    fs::path file_out = args.get("-o");
    file_out.replace_extension(file_target.extension());
    cv::imwrite(file_out.string(), result);
    return 0;
}

int main(int argc, char **argv)
{
    argparse::ArgumentParser args("PhotoFaceSwap", "0.1.0");

    args.add_argument("source_img")
        .help(
            "input file image on second"
            " positional argument or using flag `-i`")
        .required();

    args.add_argument("target_img")
        .help("target file image tobe face swaped")
        .required();

    args.add_argument("-p", "--pred")
        .help(
            "trained model file from: "
            "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2");

    args.add_argument("-g", "--gui")
        .help("run program in gui mode")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("-s", "--save")
        .help("save prediction points result from source and target image")
        .default_value(false)
        .implicit_value(true);

    args.add_argument("-d", "--debug")
        .help("verbose output in terminal")
        .default_value(2)
        .scan<'d', int>();

    args.add_argument("-o", "--output")
        .help("specified output image")
        .default_value(std::string("./output_swap.png"));

    try
    {
        args.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << termcolor::bright_red << err.what() << termcolor::reset
                  << std::endl;
        std::cerr << "-h/--help for help\n";
        std::exit(1);
    }
    return MainCli(args);
}
