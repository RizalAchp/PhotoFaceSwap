#include "gui/FaceSwapGui.hpp"

#if defined(_MSC_VER) || defined(_WIN32)
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    UNREFERENCED_PARAMETER(hInst);
    UNREFERENCED_PARAMETER(hInstPrev);
    UNREFERENCED_PARAMETER(cmdline);
    UNREFERENCED_PARAMETER(cmdshow);

    cv::InitPointDectector();
    PhotoFaceSwapApplication::Config config;
    config.title     = "PhotoFaceSwap Gui";
    config.vsync     = true;
    config.width     = 1280;
    config.height    = 720;
    config.resizable = true;
    config.background = mahi::gui::Colors::Black;
    PhotoFaceSwapApplication app(config);
    app.run();

    return 0;
}

#else
int main(void)
{
    cv::InitPointDectector();
    PhotoFaceSwapApplication::Config config;
    config.title     = "PhotoFaceSwap Gui";
    config.vsync     = true;
    config.width     = 1280;
    config.height    = 720;
    config.resizable = true;
    config.background = mahi::gui::Colors::Black;
    PhotoFaceSwapApplication app(config);
    app.run();

    return 0;
}
#endif