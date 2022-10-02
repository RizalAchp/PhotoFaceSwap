#include "gui/FaceSwapGui.hpp"

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
