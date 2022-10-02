
#include "FaceSwapGui.hpp"

#include "ImagePoints.hpp"
#include "Mahi/Util/Logging/Log.hpp"

using namespace mahi;
using namespace mahi::util;

PhotoFaceSwapApplication::PhotoFaceSwapApplication(
    const PhotoFaceSwapApplication::Config &config)
    : Application(config)
    , m_Images()
    , logger()
    , mtx()
    , outs()
    , font_small(AddDefaultFont("Roboto Mono Regular",
                                gui::RobotoMono_Regular_ttf,
                                gui::RobotoMono_Regular_ttf_len, 15))

    , font_medium(AddDefaultFont("Roboto Mono Medium",
                                 gui::RobotoMono_Regular_ttf,
                                 gui::RobotoMono_Regular_ttf_len, 24))
    , font_big(AddDefaultFont("Roboto Mono Bold", gui::RobotoMono_Bold_ttf,
                              gui::RobotoMono_Bold_ttf_len, 30))
    , mSize(1280, 720)
{
    // disable viewports
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    // connect to the file drop event
    on_file_drop.connect(this, &PhotoFaceSwapApplication::fileDropHandler);
    on_window_resized.connect(this,
                              &PhotoFaceSwapApplication::resizeWindowHandler);
    on_window_closing.connect(this,
                              &PhotoFaceSwapApplication::closeWindowHandler);
    on_error.connect(
        [&](int code, const std::string &err)
        { logger.AddLog("[ERROR]: %d - %s \n", code, err.c_str()); });

    auto &stle             = ImGui::GetStyle();
    stle.WindowPadding     = {5.f, 5.f};
    stle.CellPadding       = {5.f, 5.f};
    stle.FramePadding      = {5.f, 5.f};
    stle.WindowBorderSize  = 0.f;
    stle.ChildBorderSize   = 2.f;
    stle.IndentSpacing     = 5.f;
    stle.ItemSpacing       = {5.f, 5.f};
    stle.ColumnsMinSpacing = 5.f;

    logger.AddLog("[DEBUG]: Initialize app.. \n");
    logger.AddLog("[DEBUG]: config: {\n");
    logger.AddLog("   -title: %s\n", config.title.c_str());
    logger.AddLog("   -widht: %d\n", config.width);
    logger.AddLog("   -height: %d\n", config.height);
    logger.AddLog("   -monitor: %d\n", config.monitor);
    logger.AddLog("}\n");
}

void PhotoFaceSwapApplication::update()
{
    auto w = (mSize.x - 15.f) * 0.5f;
    auto h = (mSize.y - 15.f) * 0.5f;

    ImGui::SetNextWindowSize({w, h}, ImGuiCond_Always);
    if (ImGui::Begin("Image View Source", NULL,
                     SetNextOverlayCorner(_PositionCorner::TopLeft, 1.f)))
    {
        if (m_Images.source.empty())
            this->ButtonFileManager(
                (const char *)ICON_FA_FILE " Open Source Image", true);
        else m_Images.DrawImage(TypeImage::Source);
        ImGui::End();
    }

    ImGui::SetNextWindowSize({w, h}, ImGuiCond_Always);
    if (ImGui::Begin("Image View Target", NULL,
                     SetNextOverlayCorner(_PositionCorner::BottomLeft, 1.f)))
    {
        if (m_Images.target.empty())
            this->ButtonFileManager(
                (const char *)ICON_FA_FILE " Open Target Image", false);
        else m_Images.DrawImage(TypeImage::Target);
        ImGui::End();
    }

    ImGui::SetNextWindowSize({w, h * 1.6f}, ImGuiCond_Always);
    auto flags_out = SetNextOverlayCorner(_PositionCorner::TopRight, 1.f);
    if (ImGui::Begin("OutputImage", NULL, flags_out))
    {
        if (m_Images.output.empty())
        {
            if (ImGui::Button("Swap Target"))
            {
                logger.AddLog("[DEBUG]: swaaping target and source image..\n");
                cv::swap(m_Images.source, m_Images.target);
                std::swap(m_Images.p_target, m_Images.p_source);
                std::swap(m_Images.texture_source, m_Images.texture_target);
            }
            if (!m_Images.source.empty() && !m_Images.target.empty())
            {
                ImGui::SameLine();
                if (ImGui::Button("Start Swapping Face"))
                {
                    logger.AddLog(
                        "[PROCESS]: processing to get convexhull of points "
                        "image\n");
                    cv::GetConvexHullPoints(m_Images.p_source,
                                            m_Images.p_target);
                    logger.AddLog("[PROCESS]: processing to swap the image\n");
                    auto convex = cv::ConvexHullPoints(m_Images.p_source,
                                                       m_Images.p_target);
                    cv::ProcessImage(m_Images.source, m_Images.target, convex,
                                     m_Images.output);

                    double scale = (h * 1.5f) / m_Images.output.size().height;
                    cv::resize(m_Images.output, m_Images.output, cv::Size(0, 0), scale, scale);
                    cv::cvtColor(m_Images.output, m_Images.output, cv::COLOR_BGR2RGBA);
                    m_Images.output.copyTo(m_Images.output);
                    m_Images.UpdateTexture(TypeImage::Output);
                }
            }
        }
        else
        {
            m_Images.DrawImage(TypeImage::Output);
        }
    }

    auto flags = SetNextOverlayCorner(_PositionCorner::BottomRight);
    ImGui::SetNextWindowSize({w, h * 0.4f}, ImGuiCond_Always);
    ImGui::PushFont(font_small);
    if (ImGui::Begin("Log PhotoFaceSwap", NULL, flags))
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
    logger.Draw("Log PhotoFaceSwap", NULL, flags);
    ImGui::PopFont();
}

void PhotoFaceSwapApplication::fileDropHandler(
    const std::vector<std::string> &paths)
{
    for (auto &path : paths)
    {
        logger.AddLog("[DEBUG]: Get Dropped file: %s\n", path.c_str());
    }
    outs = paths;
}

void PhotoFaceSwapApplication::resizeWindowHandler(int w, int h)
{
    logger.AddLog("[DEBUG]: window sizing(widht: %d, height: %d)\n", w, h);
    mSize.x = w;
    mSize.y = h;

    m_Images.UpdateTexture(TypeImage::Source);
    m_Images.UpdateTexture(TypeImage::Target);
    m_Images.UpdateTexture(TypeImage::Output);
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

bool PhotoFaceSwapApplication::closeWindowHandler() { return true; }

void PhotoFaceSwapApplication::ButtonFileManager(const char *name,
                                                 bool issource)
{
    auto height = ImGui::GetContentRegionAvail().y - 35.f;
    if (ImGui::Button(name))
    {
        logger.AddLog("[DEBUG]: clicking button for Opening file: %s\n", name);
        std::string out;
        if (gui::open_dialog(out, {{"Images", "png,jpg,jpeg,webp,bmp"}}) ==
            gui::DialogResult::DialogOkay)
        {
            logger.AddLog("[DEBUG]: got file: %s\n", out.c_str());
            if (issource) m_Images.OpenImage(TypeImage::Source, out, height);
            else m_Images.OpenImage(TypeImage::Target, out, height);
            logger.AddLog("[DEBUG]: got image size(widht: %d, height: %d)\n",
                          m_Images.target.cols, m_Images.target.rows);
        }
        else logger.AddLog("[DEBUG]: cencelled file dialog\n");
    }
}

ImGuiWindowFlags SetNextOverlayCorner(PositionCorner corner, float alpha)
{
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

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

    ImGui::SetNextWindowBgAlpha(alpha);  // Transparent background
    return window_flags;
}
