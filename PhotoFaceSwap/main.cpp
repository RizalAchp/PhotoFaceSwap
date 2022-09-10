#include <PhotoFaceSwap.hpp>
#include <TermColor.hpp>

int main(int argc, char **argv)
{
    argparse::ArgumentParser args("PhotoFaceSwap", "0.1.0");

    args.add_argument("predictor")
        .help(
            "trained model file from: "
            "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2")
        .required();

    args.add_argument("source_img")
        .help(
            "input file image on second"
            " positional argument or using flag `-i`")
        .required();

    args.add_argument("target_img")
        .help("target file image tobe face swaped")
        .required();

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

    if (args["-g"] == true) return MainCli(args);

    return MainCli(args);
}
