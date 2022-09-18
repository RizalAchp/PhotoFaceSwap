#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>
#include <PhotoFaceSwap.hpp>
#include <mutex>
#include <thread>

#include "Mahi/Gui/Fonts.hpp"
#include "Mahi/Gui/Icons/IconsFontAwesome5.hpp"

using namespace mahi;
using namespace mahi::util;

typedef int PositionCorner;
enum _PositionCorner : PositionCorner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
};

ImFont *AddDefaultFont(const char *namefont, unsigned char *font,
                       size_t size_font, float pixel_size);
ImGuiWindowFlags SetNextOverlayCorner(PositionCorner corner);

class PhotoFaceSwapApplication : public gui::Application
{
   private:
    ImFont *font_small;
    ImFont *font_medium;
    ImFont *font_big;

    ImVec2 mSize;

   public:
    PhotoFaceSwapApplication(const PhotoFaceSwapApplication::Config &config)
        : Application(config), mSize(1280, 720)
    {
        font_small =
            AddDefaultFont("Roboto Mono Regular", gui::RobotoMono_Regular_ttf,
                           gui::RobotoMono_Regular_ttf_len, 15);
        font_medium =
            AddDefaultFont("Roboto Mono Medium", gui::RobotoMono_Regular_ttf,
                           gui::RobotoMono_Regular_ttf_len, 24);
        font_big = AddDefaultFont("Roboto Mono Bold", gui::RobotoMono_Bold_ttf,
                                  gui::RobotoMono_Bold_ttf_len, 30);
        // disable viewports
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
        // connect to the file drop event
        on_file_drop.connect(this, &PhotoFaceSwapApplication::fileDropHandler);
        on_window_resized.connect(
            this, &PhotoFaceSwapApplication::resizeWindowHandler);

        auto &stle             = ImGui::GetStyle();
        stle.WindowPadding     = {5.f, 5.f};
        stle.CellPadding       = {5.f, 5.f};
        stle.FramePadding      = {5.f, 5.f};
        stle.WindowBorderSize  = 0.f;
        stle.ChildBorderSize   = 1.f;
        stle.IndentSpacing     = 5.f;
        stle.ItemSpacing       = {5.f, 5.f};
        stle.ColumnsMinSpacing = 5.f;
    }

    void update() override
    {
        auto &stle = ImGui::GetStyle();
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
        ImGui::SetNextWindowSize(mSize, ImGuiCond_Always);
        if (ImGui::Begin("Image View", nullptr,
                         ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoBringToFrontOnFocus))
        {
            auto sz = ImGui::GetWindowSize();
            auto w = (sz.x - (stle.FramePadding.x + stle.WindowPadding.x * 2)) *
                     0.5f;
            auto h = (sz.y - (stle.FramePadding.x + stle.WindowPadding.y * 2)) *
                     0.5f;

            ImGui::BeginChild(
                "Source Image", {w, h},
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);
            if (ImGui::Button((const char *)ICON_FA_FILE " Open Source Image"))
            {
                auto func = [this]()
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (gui::open_dialog(out, {{"Headers", "hpp,h,inl"},
                                               {"Source code", "cpp,c,cc"}}) ==
                        gui::DialogResult::DialogOkay)
                        LOG_DEBUG("Path: {%s}", out.c_str());
                };
                std::thread thrd(func);
                thrd.detach();
            }
            ImGui::EndChild();
            ImGui::BeginChild(
                "Target Image", {w, h},
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);
            if (ImGui::Button((const char *)ICON_FA_FILE " Open Target Image"))
            {
                auto func = [this]()
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (gui::open_dialog(out, {{"Headers", "hpp,h,inl"},
                                               {"Source code", "cpp,c,cc"}}) ==
                        gui::DialogResult::DialogOkay)
                        LOG_DEBUG("Path: {%s}", out.c_str());
                };
                std::thread thrd(func);
                thrd.detach();
            }
            ImGui::EndChild();
            ImGui::End();
        }

        auto flags = SetNextOverlayCorner(_PositionCorner::TopRight);
        ImGui::Begin("Dialog", nullptr, flags);
        ImGui::PushFont(font_big);
        if (ImGui::Button((const char *)ICON_FA_SAVE " Save Output Image"))
        {
            auto func = [this]()
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (gui::save_dialog(
                        out,
                        {{"Headers", "hpp,h,inl"}, {"Source code", "cpp,c,cc"}},
                        "", "my_file") == gui::DialogResult::DialogOkay)
                    LOG_DEBUG("Path: {%s}", out.c_str());
            };
            std::thread thrd(func);
            thrd.detach();
        }
        ImGui::SameLine();
        if (ImGui::Button((const char *)ICON_FA_COPY " Open Multiple"))
        {
            auto func = [this]()
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (gui::open_dialog(outs, {{"Headers", "hpp,h,inl"},
                                            {"Source code", "cpp,c,cc"}}) ==
                    gui::DialogResult::DialogOkay)
                {
                    for (auto &o : outs) LOG_DEBUG("Path: {%s}", o.c_str());
                }
            };
            std::thread thrd(func);
            thrd.detach();
        }
        ImGui::PopFont();

        ImGui::End();

        ImGui::PushFont(font_small);
        auto window_flags = SetNextOverlayCorner(_PositionCorner::BottomRight);
        if (ImGui::Begin("Example: Simple overlay", NULL, window_flags))
        {
            if (ImGui::Button((const char *)ICON_FA_IMAGES " PhotoFaceSwap"))
                gui::open_url("https://github.com/RizalAchp/PhotoFaceSwap");
            ImGui::SameLine();
            if (ImGui::Button((const char *)ICON_FA_GITHUB " RizalAchp"))
                gui::open_url("https://github.com/RizalAchp");
            ImGui::SameLine();
            if (ImGui::Button((const char *)ICON_FA_PAPER_PLANE " Email"))
                gui::open_email("rizal.ahmadp@gmail.com", "I love you!");
            ImGui::End();
        }
        ImGui::PopFont();
    }

    void fileDropHandler(const std::vector<std::string> &paths)
    {
        outs = paths;
    }

    void resizeWindowHandler(int w, int h)
    {
        mSize.x = w;
        mSize.y = h;
    }

    std::mutex mtx;
    std::string out;
    std::vector<std::string> outs;
};

int main(void)
{
    PhotoFaceSwapApplication::Config config;
    config.title     = "PhotoFaceSwap Gui";
    config.vsync     = true;
    config.width     = 1280;
    config.height    = 720;
    config.resizable = true;
    PhotoFaceSwapApplication demo(config);
    demo.run();
    return 0;
}

ImFont *AddDefaultFont(const char *namefont, unsigned char *font,
                       size_t size_font, float pixel_size)
{
    ImGuiIO &io = ImGui::GetIO();

    ImFontConfig font_cfg;
    font_cfg.PixelSnapH  = true;
    font_cfg.OversampleH = 1, font_cfg.OversampleV = 1;
    font_cfg.FontDataOwnedByAtlas = false;
    strcpy(font_cfg.Name, namefont);
    io.Fonts->AddFontFromMemoryTTF(font, size_font, round(pixel_size * 1.f),
                                   &font_cfg);

    ImFontConfig icons_config;
    icons_config.MergeMode            = true;
    icons_config.PixelSnapH           = true;
    icons_config.GlyphMinAdvanceX     = 14.0f;
    icons_config.GlyphOffset          = ImVec2(0, 0);
    icons_config.OversampleH          = 1;
    icons_config.OversampleV          = 1;
    icons_config.FontDataOwnedByAtlas = false;

    // merge in icons from font awesome 5
    static const ImWchar fa_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromMemoryTTF(
        gui::fa_solid_900_ttf, gui::fa_solid_900_ttf_len,
        round(pixel_size * 1.f), &icons_config, fa_ranges);

    // merge in icons from font awesome 5 brands
    static const ImWchar fab_ranges[] = {ICON_MIN_FAB, ICON_MAX_FAB, 0};
    ImFont *font_out                  = io.Fonts->AddFontFromMemoryTTF(
                         gui::fa_brands_400_ttf, gui::fa_brands_400_ttf_len,
                         round(pixel_size * 1.f), &icons_config, fab_ranges);
    return font_out;
}

ImGuiWindowFlags SetNextOverlayCorner(PositionCorner corner)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove;
    const float PAD               = 5.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos               = viewport->Pos;
    ImVec2 work_size              = viewport->Size;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x =
        (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y =
        (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::SetNextWindowBgAlpha(0.2f);  // Transparent background
    return window_flags;
}
