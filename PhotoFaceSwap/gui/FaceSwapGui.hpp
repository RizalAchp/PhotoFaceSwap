#pragma once

#ifndef __FACE_SWAP_GUI_HEADER__
#define __FACE_SWAP_GUI_HEADER__

#include "Image.hpp"

typedef int PositionCorner;
enum _PositionCorner : PositionCorner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

struct ImFont;

ImFont *AddDefaultFont(const char *namefont, unsigned char *font,
                       size_t size_font, float pixel_size);
ImGuiWindowFlags SetNextOverlayCorner(PositionCorner corner, float alpha = 0.2f);

class PhotoFaceSwapApplication : public mahi::gui::Application
{
   public:
    PhotoFaceSwapApplication(const PhotoFaceSwapApplication::Config &config);
    void update() override;
    void fileDropHandler(const std::vector<std::string> &paths);
    void resizeWindowHandler(int w, int h);
    bool closeWindowHandler();

    void ButtonFileManager(const char *name, bool issource);


   public:
    ImageProcessing m_Images;
    AppLog logger;
    std::mutex mtx;
    std::vector<std::string> outs;

   private:
    ImFont *font_small;
    ImFont *font_medium;
    ImFont *font_big;
    ImVec2 mSize;
};

#endif
