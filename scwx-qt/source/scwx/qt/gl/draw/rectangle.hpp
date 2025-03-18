#pragma once

#include <scwx/qt/gl/gl_context.hpp>
#include <scwx/qt/gl/draw/draw_item.hpp>

#include <boost/gil.hpp>

namespace scwx
{
namespace qt
{
namespace gl
{
namespace draw
{

class Rectangle : public DrawItem
{
public:
   explicit Rectangle(std::shared_ptr<GlContext> context);
   ~Rectangle();

   Rectangle(const Rectangle&)            = delete;
   Rectangle& operator=(const Rectangle&) = delete;

   Rectangle(Rectangle&&) noexcept;
   Rectangle& operator=(Rectangle&&) noexcept;

   void Initialize() override;
   void Render(const QMapLibre::CustomLayerRenderParameters& params) override;
   void Deinitialize() override;

   void SetBorder(float width, boost::gil::rgba8_pixel_t color);
   void SetFill(boost::gil::rgba8_pixel_t color);
   void SetPosition(float x, float y, float z = 0.0f);
   void SetSize(float width, float height);
   void SetVisible(bool visible);

private:
   class Impl;

   std::unique_ptr<Impl> p;
};

} // namespace draw
} // namespace gl
} // namespace qt
} // namespace scwx
