#include <imgui.h>

#include <Image.hpp>
#include <Mahi/Gui/Icons/IconsFontAwesome5.hpp>
#include <Mahi/Gui/Native.hpp>
#include <opencv2/imgproc.hpp>


ImageSwap::~ImageSwap() { this->Close(); }

void ImageSwap::Draw() noexcept(false)
{
    using namespace ImGui;
    using mahi::gui::Vec2;
    Vec2 size = Vec2(this->resized.cols, this->resized.rows);
    Vec2 pos  = GetCursorPos() + (GetContentRegionAvail() - size) * 0.5f;

    SetCursorPos(pos);
    Image(reinterpret_cast<void *>(static_cast<intptr_t>(this->texture)), size);

    if (this->points.empty()) return;
    auto drw  = GetWindowDrawList();
    Vec2 ppos = (pos + GetWindowPos());
    for (auto &point : this->points_show)
    {
        for (auto it = point.begin(); it != point.end(); ++it)
        {
            auto nx = it + 1;
            drw->AddLine(Vec2(it->x, it->y) + ppos, Vec2(nx->x, nx->y) + ppos,
                         IM_COL32(255, 0, 0, 255), 2.f);
        }
    }
}

bool ImageSwap::Open(const std::string &file) noexcept(false)
{
    this->raw = cv::imread(file);
    if (this->raw.empty()) return false;
    return true;
}

void ImageSwap::Close() noexcept
{
    this->raw.release();
    this->resized.release();
    this->points.clear();
    this->points_show.clear();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);
}
void ImageSwap::Flip(int code)
{
    cv::flip(this->raw, this->raw, code);
    cv::flip(this->resized, this->resized, code);
    this->points.clear();
    this->points_show.clear();

    this->UpdateTexture();
}
void ImageSwap::Resize(const mahi::gui::Vec2 &size) noexcept(false)
{
    this->raw.copyTo(this->resized);
    if (this->raw.rows == size.y) return;
    double y     = (size.y - 35) / this->raw.size().height;
    auto ip    = this->raw.rows > size.y ? cv::INTER_AREA : cv::INTER_LINEAR;
    double yraw  = 1280.0 / this->raw.size().height;
    auto ipraw = this->raw.rows > 1280.0 ? cv::INTER_AREA : cv::INTER_LINEAR;
    cv::resize(this->raw, this->raw, cv::Size(0, 0), yraw, yraw, ipraw);
    cv::resize(this->resized, this->resized, cv::Size(0, 0), y, y, ip);
    cv::cvtColor(this->resized, this->resized, cv::COLOR_BGR2RGBA);
    this->UpdateTexture();
}

void ImageSwap::UpdateTexture() noexcept(false)
{
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->resized.cols,
                 this->resized.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 this->resized.data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

bool ImageSwap::ProcessPoints() noexcept(false)
{
    cv::GetPointImage(this->raw, this->points, false);
    cv::GetPointImage(this->resized, this->points_show, true);
    if (this->points.empty() || this->points_show.empty())
    {
        return false;
    }
    return true;
}

/// void ImageProcessing::OpenImage(TypeImage type, const std::string &file,
///                                 float height)
/// {
///     cv::Mat tmp = cv::imread(file);
///     if (tmp.empty()) return;
///     switch (type)
///     {
///         case Source:
///         {
///             double scale = height / tmp.size().height;
///             cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
///             cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
///             tmp.copyTo(source);

///             UpdateTexture(TypeImage::Source);
///         }
///         break;
///         case Target:
///         {
///             double scale = height / tmp.size().height;
///             cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
///             cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
///             tmp.copyTo(target);
///             UpdateTexture(TypeImage::Target);
///         }
///         break;
///         case Output:
///         {
///             double scale = height / tmp.size().height;
///             cv::resize(tmp, tmp, cv::Size(0, 0), scale, scale);
///             cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGBA);
///             tmp.copyTo(output);
///             UpdateTexture(TypeImage::Output);
///         }
///         break;
///     }
/// }
