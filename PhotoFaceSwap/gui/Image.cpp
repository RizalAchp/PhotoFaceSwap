#include "Image.hpp"

#include "imgui.h"
void ImageProcessing::ResizeImage(TypeImage type, float height)
{
    switch (type)
    {
        case Source:
        {
            double scale = height / source.size().height;
            cv::resize(output, output, cv::Size(0, 0), scale, scale);
        }
        break;
        case Target:

        {
            double scale = height / target.size().height;
            cv::resize(output, output, cv::Size(0, 0), scale, scale);
        }
        break;
        case Output:

        {
            double scale = height / output.size().height;
            cv::resize(output, source, cv::Size(0, 0), scale, scale);
        }
        break;
    }
}
void ImageProcessing::ProcessPoints(TypeImage type)
{
    switch (type)
    {
        case Source:
            cv::GetPointImage(source, p_source);
            break;
        case Target:
            cv::GetPointImage(target, p_target);
            break;
        default:
            break;
    }
}

inline static void UpdateTexture_(unsigned char *data, int width, int height,
                                  GLuint *texture_out)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    *texture_out = texture;
}

void ImageProcessing::UpdateTexture(TypeImage type)
{
    switch (type)
    {
        case Source:
            UpdateTexture_(source.data, source.cols, source.rows,
                           &texture_source);
            break;
        case Target:
            UpdateTexture_(target.data, target.cols, target.rows,
                           &texture_target);
            break;
        case Output:
            UpdateTexture_(output.data, output.cols, output.rows,
                           &texture_output);
            break;
    }
}

inline static void DrawImage_(GLuint &texture, const mahi::gui::Vec2 &size,
                              const cv::ImagePoints2f *point,
                              const mahi::gui::Vec2 &pos,
                              const mahi::gui::Vec2 &optional = {0, 0},
                              const mahi::gui::Vec2 &padding  = {0, 0})
{
    ImGui::SetCursorPos(pos);
    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture)),
                 size);

    if (point == NULL) return;
    if (point->empty()) return;
    auto drw = ImGui::GetWindowDrawList();
    for (auto it = point->begin(); it != point->end(); ++it)
    {
        auto next = it + 1;
        drw->AddLine(
            (pos + optional + mahi::gui::Vec2(it->x, it->y)) + padding,
            (pos + optional + mahi::gui::Vec2(next->x, next->y)) + padding,
            ImGui::ColorConvertFloat4ToU32(mahi::gui::Colors::Red), 2.f);
    }
}

void ImageProcessing::DrawImage(TypeImage type)
{
    switch (type)
    {
        case TypeImage::Source:
        {
            if (ImGui::Button("Close Image##source"))
            {
                source.release();
                p_source.clear();
                glDeleteTextures(1, &texture_source);
            }
            ImGui::SameLine();
            if (ImGui::Button("GetPoints Image Source"))
            {
                ProcessPoints(TypeImage::Source);
            }

            auto size = mahi::gui::Vec2((float)source.cols, (float)source.rows);
            auto pos  = ImGui::GetCursorPos() +
                       (ImGui::GetContentRegionAvail() - size) * 0.5f;
            DrawImage_(texture_source, size, &p_source, pos, {0, 0},
                       ImGui::GetStyle().FramePadding);
        }
        break;
        case TypeImage::Target:
        {
            if (ImGui::Button("Close Image##target"))
            {
                target.release();
                p_target.clear();
                glDeleteTextures(1, &texture_target);
            }
            ImGui::SameLine();
            if (ImGui::Button("GetPoints Image Target"))
            {
                ProcessPoints(TypeImage::Target);
            }

            auto size = mahi::gui::Vec2((float)target.cols, (float)target.rows);
            auto pos  = ImGui::GetCursorPos() +
                       (ImGui::GetContentRegionAvail() - size) * 0.5f;
            DrawImage_(texture_target, size, &p_target, pos,
                       ImGui::GetWindowPos());
        }
        break;
        case TypeImage::Output:
        {
            if (ImGui::Button("Close Image##output"))
            {
                output.release();
                glDeleteTextures(1, &texture_output);
            }
            auto size = mahi::gui::Vec2((float)output.cols, (float)output.rows);
            auto pos  = ImGui::GetCursorPos() +
                       (ImGui::GetContentRegionAvail() - size) * 0.5f;
            DrawImage_(texture_output, size, NULL, pos);
        }
        break;
    }
}

void ImageProcessing::OpenImage(TypeImage type, const std::string &file,
                                float height)
{
    cv::Mat tmp = cv::imread(file);
    if (tmp.empty()) return;
    switch (type)
    {
        case Source:
        {
            double scale = height / tmp.size().height;
            cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
            tmp.copyTo(source);

            UpdateTexture(TypeImage::Source);
        }
        break;
        case Target:
        {
            double scale = height / tmp.size().height;
            cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
            tmp.copyTo(target);
            UpdateTexture(TypeImage::Target);
        }
        break;
        case Output:
        {
            double scale = height / tmp.size().height;
            cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
            cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
            tmp.copyTo(output);
            UpdateTexture(TypeImage::Output);
        }
        break;
    }
}
