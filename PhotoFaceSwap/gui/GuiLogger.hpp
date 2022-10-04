#pragma once

#include <Logger.hpp>
#include <Mahi/Gui.hpp>
#include <Mahi/Gui/Fonts.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Util.hpp>
struct AppLog : public Writer
{
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;
    bool AutoScroll;

    AppLog();
    ~AppLog() override;
    void Clear();
    void AddLog(const std::string &msg) override;
    void Draw(const char *title, bool *p_open = NULL, int flags = 0) override;
};
