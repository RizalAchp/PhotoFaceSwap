#include <FaceSwapGui.hpp>

using namespace mahi;
using namespace mahi::util;

PhotoFaceSwapApplication::PhotoFaceSwapApplication(
    const PhotoFaceSwapApplication::Config &config)
    : Application(config)
    , Source()
    , Target()
    , Output()
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
    LoggerPhotoFaceSwap::Init<AppLog>();
    setLogLevel(LogLevel::Debug);
    // disable viewports
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
    // connect to the file drop event
    on_file_drop.connect(this, &PhotoFaceSwapApplication::fileDropHandler);
    on_window_resized.connect(this,
                              &PhotoFaceSwapApplication::resizeWindowHandler);
    on_window_closing.connect(this,
                              &PhotoFaceSwapApplication::closeWindowHandler);
    on_error.connect([&](int code, const std::string &err)
                     { LOG_ERROR("[ERROR]: %d - %s \n", code, err.c_str()); });

    auto &stle             = ImGui::GetStyle();
    stle.WindowPadding     = {5.f, 5.f};
    stle.CellPadding       = {5.f, 5.f};
    stle.FramePadding      = {5.f, 5.f};
    stle.WindowBorderSize  = 0.f;
    stle.ChildBorderSize   = 2.f;
    stle.IndentSpacing     = 5.f;
    stle.ItemSpacing       = {5.f, 5.f};
    stle.ColumnsMinSpacing = 5.f;

    LOG_DEBUG("[DEBUG]: Initialize app.. \n");
    LOG_DEBUG("[DEBUG]: config: {\n");
    LOG_DEBUG("   -title: %s\n", config.title.c_str());
    LOG_DEBUG("   -widht: %d\n", config.width);
    LOG_DEBUG("   -height: %d\n", config.height);
    LOG_DEBUG("   -monitor: %d\n", config.monitor);
    LOG_DEBUG("}\n");
}

void PhotoFaceSwapApplication::update()
{
    auto w = (mSize.x - 15.f) * 0.5f;
    auto h = (mSize.y - 15.f) * 0.5f;

    ImGui::SetNextWindowSize({w, h}, ImGuiCond_Always);
    if (ImGui::Begin("Image View Source", NULL,
                     SetNextOverlayCorner(_PositionCorner::TopLeft, 1.f)))
    {
        if (Source.resized.empty() || Source.raw.empty())
        {
            if (!outs.empty())
            {
                if (Source.Open(outs))
                {
                    Source.Resize(ImGui::GetContentRegionAvail());
                }
                else
                {
                    this->errorMsg.clear();
                    this->errorMsg = cv::format("Cannot Open Image on file: %s",
                                                outs.c_str());
                    ImGui::OpenPopup("ERROR!");
                }
                outs.clear();
            }

            this->ButtonFileManager(
                (const char *)ICON_FA_FILE " Open Source Image", true);
        }
        else
        {
            if (ImGui::Button("Close Image##source"))
            {
                Source.Close();
                outs.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("GetPoints Image##source"))
            {
                if (!Source.ProcessPoints())
                {
                    this->errorMsg.clear();
                    this->errorMsg =
                        "Thereis No Face Destected on Source Image!";
                    ImGui::OpenPopup("ERROR!");
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("RemovePoints##source"))
            {
                Source.points_show.clear();
                Source.points.clear();
            }
            ImGui::SameLine();
            static int flipcode = FlipCode::Horizontal;
            ImGui::PushItemWidth(100);
            if (ImGui::Combo("FlipImage##source", &flipcode, FLIP_CODE_NAME,
                             IM_ARRAYSIZE(FLIP_CODE_NAME)))
            {
                Source.Flip(flipcode);
            }

            Source.Draw();
        }
        ImGui::End();
    }

    ImGui::SetNextWindowSize({w, h}, ImGuiCond_Always);
    if (ImGui::Begin("Image View Target", NULL,
                     SetNextOverlayCorner(_PositionCorner::BottomLeft, 1.f)))
    {
        if (Target.resized.empty() || Target.raw.empty())
        {
            if (!outs.empty())
            {
                if (Target.Open(outs))
                    Target.Resize(ImGui::GetContentRegionAvail());
                else
                {
                    this->errorMsg.clear();
                    this->errorMsg = cv::format("Cannot Open Image on file: %s",
                                                outs.c_str());
                    ImGui::OpenPopup("ERROR!");
                }
                outs.clear();
            }

            this->ButtonFileManager(
                (const char *)ICON_FA_FILE " Open Target Image", false);
        }
        else
        {
            if (ImGui::Button("Close Image##target"))
            {
                Target.Close();
                outs.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("GetPoints Image##target"))
            {
                if (!Target.ProcessPoints())
                {
                    this->errorMsg.clear();
                    this->errorMsg =
                        "Thereis No Face Destected on Target Image!";
                    ImGui::OpenPopup("ERROR!");
                }
            }
            Target.Draw();
        }
        ImGui::End();
    }

    ImGui::SetNextWindowSize({w, h * 1.4f}, ImGuiCond_Always);
    auto flags_out = SetNextOverlayCorner(_PositionCorner::TopRight, 1.f);
    if (ImGui::Begin("OutputImage", NULL, flags_out))
    {
        if (Output.raw.empty())
        {
            if (ImGui::Button("Swap Target"))
            {
                LOG_DEBUG("swaaping target and source image..\n");
                cv::swap(Source.raw, Target.raw);
                cv::swap(Source.resized, Target.resized);
                std::swap(Source.points, Target.points);
                std::swap(Source.points_show, Target.points_show);
                std::swap(Source.point_selected, Target.point_selected);
                std::swap(Source.texture, Target.texture);
                Source.UpdateTexture();
                Target.UpdateTexture();
            }
            if (!Source.raw.empty() && !Target.raw.empty())
            {
                if (ImGui::Button("Start Swapping Face"))
                {
                    ImGui::SameLine();
                    try
                    {
                        LOG_DEBUG(
                            "PROCESS: processing to get convexhull of "
                            "points "
                            "image\n");
                        if (Source.points.empty() || Target.points.empty())
                        {
                            Source.ProcessPoints();
                            Target.ProcessPoints();
                        }
                        cv::GetConvexHullPoints(Source.points, Target.points);
                        LOG_DEBUG("PROCESS: processing to swap the image\n");
                        cv::ProcessImage(Source.raw, Target.raw,
                                         Source.GetPointSelected(),
                                         Target.GetPointSelected(), Output.raw);

                        Output.Resize(ImGui::GetContentRegionAvail());
                        Output.UpdateTexture();
                    }
                    catch (const cv::Exception &e)
                    {
                        errorMsg.clear();
                        errorMsg = e.what();
                        ImGui::OpenPopup("ERROR!");
                    }
                    catch (const std::exception &e)
                    {
                        errorMsg.clear();
                        errorMsg = e.what();
                        ImGui::OpenPopup("ERROR!");
                    }
                }
            }
        }
        else
        {
            if (ImGui::Button((const char *)ICON_FA_TRASH "Close All"))
            {
                LOG_DEBUG("Closing all image..\n");
                Source.Close();
                Target.Close();
                Output.Close();
                outs.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button((const char *)ICON_FA_TRASH "Close##output"))
            {
                LOG_DEBUG("Closing all image..\n");
                Output.Close();
                outs.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button((const char *)ICON_FA_IMAGES "Save Output##1"))
            {
                std::thread thrd(
                    [this]()
                    {
                        std::string out;
                        std::lock_guard<std::mutex> lock(mtx);
                        if (gui::save_dialog(
                                out, {{"Images", "png,jpg,jpeg,webp,bmp"}}, "",
                                "output.png") == gui::DialogResult::DialogOkay)
                        {
                            if (cv::imwrite(out, this->Output.raw))
                                LOG_DEBUG("Success saving Image on: %s",
                                          out.c_str());
                            else
                            {
                                errorMsg.clear();
                                errorMsg = cv::format(
                                    "Error on Saving Output Image to file: %s",
                                    out.c_str());
                                LOG_ERROR("%s", errorMsg.c_str());
                                ImGui::OpenPopup("ERROR!");
                            }
                        }
                    });

                thrd.detach();
            }
            Output.Draw();
        }
    }

    auto flags = SetNextOverlayCorner(_PositionCorner::BottomRight);
    ImGui::SetNextWindowSize({w, h * 0.6f}, ImGuiCond_Always);
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
    LOG_DRAW("Log PhotoFaceSwap", NULL, flags);
    ImGui::PopFont();

    if (ImGui::BeginPopupModal("ERROR!", NULL))
    {
        ImGui::Text("Got Error: %s", errorMsg.c_str());
        if (ImGui::Button("Ok")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void PhotoFaceSwapApplication::fileDropHandler(
    const std::vector<std::string> &paths)
{
    for (auto &path : paths)
    {
        LOG_DEBUG("Get Dropped file: %s\n", path.c_str());
    }
    outs = paths.front();
}

void PhotoFaceSwapApplication::resizeWindowHandler(int w, int h)
{
    LOG_DEBUG("window sizing(widht: %d, height: %d)\n", w, h);
    mSize.x = w;
    mSize.y = h;
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
    if (ImGui::Button(name))
    {
        LOG_DEBUG("clicking button for Opening file: %s\n", name);
        std::string out;
        if (gui::open_dialog(out, {{"Images", "png,jpg,jpeg,webp,bmp"}}) ==
            gui::DialogResult::DialogOkay)
        {
            LOG_DEBUG("got file: %s\n", out.c_str());
            if (issource)
            {
                if (Source.Open(out))
                {
                    Source.Resize(ImGui::GetContentRegionAvail());
                }
                else
                {
                    this->errorMsg.clear();
                    this->errorMsg = cv::format("Cannot Open Image on file: %s",
                                                out.c_str());
                    ImGui::OpenPopup("ERROR!");
                }
            }
            else
            {
                if (Target.Open(out))
                {
                    Target.Resize(ImGui::GetContentRegionAvail());
                }
                else
                {
                    this->errorMsg.clear();
                    this->errorMsg = cv::format("Cannot Open Image on file: %s",
                                                out.c_str());
                    ImGui::OpenPopup("ERROR!");
                }
            }
        }
        else LOG_DEBUG("cencelled file dialog\n");
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
