#include "tvg_renderer.hpp"
#include "rive/math/vec2d.hpp"
#include "rive/shapes/paint/color.hpp"

using namespace rive;


void TvgRenderPath::fillRule(FillRule value)
{
   switch (value)
   {
      case FillRule::evenOdd:
         tvgShape->fill(tvg::FillRule::EvenOdd);
         break;
      case FillRule::nonZero:
         tvgShape->fill(tvg::FillRule::Winding);
         break;
   }
}

Point transformCoord(const Point pt, const Mat2D &transform)
{
   Matrix m = {1, 0, 0, 0, 1, 0, 0, 0, 1};
   m.e11 = transform[0];
   m.e12 = transform[2];
   m.e13 = transform[4];
   m.e21 = transform[1];
   m.e22 = transform[3];
   m.e23 = transform[5];

   return {pt.x * m.e11 + pt.y * m.e12 + m.e13, pt.x * m.e21 + pt.y * m.e22 + m.e23};
}

void TvgRenderPath::reset()
{
   tvgShape->reset();
}

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   const Point* pts;
   auto ptsCnt = static_cast<TvgRenderPath*>(path)->tvgShape->pathCoords(&pts);
   if (!pts) return;

   const PathCommand* cmds;
   auto cmdCnt = static_cast<TvgRenderPath*>(path)->tvgShape->pathCommands(&cmds);
   if (!cmds) return;

   //Capture the last coordinates
   Point* pts2;
   auto ptsCnt2 = tvgShape->pathCoords(const_cast<const Point**>(&pts2));

   tvgShape->appendPath(cmds, cmdCnt, pts, ptsCnt);

   //Immediate Transform for the newly appended
   Point* pts3;
   auto ptsCnt3 = tvgShape->pathCoords(const_cast<const Point**>(&pts3));

   for (unsigned i = ptsCnt2; i < ptsCnt3; ++i)
   {
      pts3[i] = transformCoord(pts3[i], transform);
   }
}

void TvgRenderPath::moveTo(float x, float y)
{
   tvgShape->moveTo(x, y);
}

void TvgRenderPath::lineTo(float x, float y)
{
   tvgShape->lineTo(x, y);
}

void TvgRenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
   tvgShape->cubicTo(ox, oy, ix, iy, x, y);
}

void TvgRenderPath::close()
{
   tvgShape->close();
}

void TvgRenderPaint::style(RenderPaintStyle style)
{
   m_Paint.style = style;
}

void TvgRenderPaint::color(unsigned int value)
{
   m_Paint.color[0] = value >> 16 & 255;
   m_Paint.color[1] = value >> 8 & 255;
   m_Paint.color[2] = value >> 0 & 255;
   m_Paint.color[3] = value >> 24 & 255;
}

void TvgRenderPaint::thickness(float value)
{
   m_Paint.thickness = value;
}

void TvgRenderPaint::join(StrokeJoin value)
{
   switch (value)
   {
      case rive::StrokeJoin::round:
         m_Paint.join = tvg::StrokeJoin::Round;
         break;
      case rive::StrokeJoin::bevel:
         m_Paint.join = tvg::StrokeJoin::Bevel;
         break;
      case rive::StrokeJoin::miter:
         m_Paint.join = tvg::StrokeJoin::Miter;
         break;
   }
}

void TvgRenderPaint::cap(StrokeCap value)
{
   switch (value)
   {
      case rive::StrokeCap::butt:
         m_Paint.cap = tvg::StrokeCap::Butt;
         break;
      case rive::StrokeCap::round:
         m_Paint.cap = tvg::StrokeCap::Round;
         break;
      case rive::StrokeCap::square:
         m_Paint.cap = tvg::StrokeCap::Square;
         break;
   }
}

void TvgRenderPaint::linearGradient(float sx, float sy, float ex, float ey)
{
   m_GradientBuilder = new TvgLinearGradientBuilder(sx, sy, ex, ey);
}

void TvgRenderPaint::radialGradient(float sx, float sy, float ex, float ey)
{
   m_GradientBuilder = new TvgRadialGradientBuilder(sx, sy, ex, ey);
}

void TvgRenderPaint::addStop(unsigned int color, float stop)
{
   m_GradientBuilder->stops.emplace_back(GradientStop(color, stop));
}

void TvgRenderPaint::completeGradient()
{
   m_GradientBuilder->make(&m_Paint);
   delete m_GradientBuilder;
}

void TvgRenderPaint::blendMode(BlendMode value)
{

}

void TvgRadialGradientBuilder::make(TvgPaint* paint)
{
   paint->isGradient = true;
   int numStops = stops.size();

   paint->gradientFill = tvg::RadialGradient::gen().release();
   float radius = Vec2D::distance(Vec2D(sx, sy), Vec2D(ex, ey));
   static_cast<RadialGradient*>(paint->gradientFill)->radial(sx, sy, radius);

   tvg::Fill::ColorStop colorStops[numStops];
   for (int i = 0; i < numStops; i++)
   {
      unsigned int value = stops[i].color;
      uint8_t r = value >> 16 & 255;
      uint8_t g = value >> 8 & 255;
      uint8_t b = value >> 0 & 255;
      uint8_t a = value >> 24 & 255;

      colorStops[i] = {stops[i].stop, r, g, b, a};
   }

   static_cast<RadialGradient*>(paint->gradientFill)->colorStops(colorStops, numStops);
}

void TvgLinearGradientBuilder::make(TvgPaint* paint)
{
   paint->isGradient = true;
   int numStops = stops.size();

   paint->gradientFill = tvg::LinearGradient::gen().release();
   static_cast<LinearGradient*>(paint->gradientFill)->linear(sx, sy, ex, ey);

   tvg::Fill::ColorStop colorStops[numStops];
   for (int i = 0; i < numStops; i++)
   {
      unsigned int value = stops[i].color;
      uint8_t r = value >> 16 & 255;
      uint8_t g = value >> 8 & 255;
      uint8_t b = value >> 0 & 255;
      uint8_t a = value >> 24 & 255;

      colorStops[i] = {stops[i].stop, r, g, b, a};
   }

   static_cast<LinearGradient*>(paint->gradientFill)->colorStops(colorStops, numStops);
}

void TvgRenderer::save()
{
    m_SavedTransforms.push(m_Transform);
}

void TvgRenderer::restore()
{
    // Check shouldn't be needed, but safest to check
    if (m_SavedTransforms.size() > 0)
    {
        m_Transform = m_SavedTransforms.top();
        m_SavedTransforms.pop();
    }
}

void TvgRenderer::transform(const Mat2D& transform)
{
   m_Transform = m_Transform * transform;
}

void TvgRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
   auto tvgShape = static_cast<TvgRenderPath*>(path)->tvgShape.get();
   auto tvgPaint = static_cast<TvgRenderPaint*>(paint)->paint();

   /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
      thorvg doesn't need to handle both, we can avoid one of them rendering... */

   if (tvgPaint->style == RenderPaintStyle::fill)
   {
      if (!tvgPaint->isGradient)
         tvgShape->fill(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      else
      {
         tvgShape->fill(unique_ptr<tvg::Fill>(tvgPaint->gradientFill->duplicate()));
      }
   }
   else if (tvgPaint->style == RenderPaintStyle::stroke)
   {
      tvgShape->stroke(tvgPaint->cap);
      tvgShape->stroke(tvgPaint->join);
      tvgShape->stroke(tvgPaint->thickness);

      if (!tvgPaint->isGradient)
         tvgShape->stroke(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      else
      {
         tvgShape->stroke(unique_ptr<tvg::Fill>(tvgPaint->gradientFill->duplicate()));
      }
   }

   if (m_ClipPath)
   {
      m_ClipPath->fill(255, 255, 255, 255);
      tvgShape->composite(unique_ptr<Shape>(static_cast<Shape*>(m_ClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      m_ClipPath = nullptr;
   }

   tvgShape->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});

   if (m_BgClipPath)
   {
      m_BgClipPath->fill(255, 255, 255, 255);
      auto scene = tvg::Scene::gen();
      scene->push(unique_ptr<Paint>(tvgShape->duplicate()));
      scene->composite(unique_ptr<Shape>(static_cast<Shape*>(m_BgClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      m_Canvas->push(move(scene));
   }
   else
      m_Canvas->push(unique_ptr<Paint>(tvgShape->duplicate()));
}


void TvgRenderer::clipPath(RenderPath* path)
{
   //Note: ClipPath transform matrix is calculated by transfrom matrix in addRenderPath function
   if (!m_BgClipPath)
   {
      m_BgClipPath = static_cast<TvgRenderPath*>(path)->tvgShape.get();
      m_BgClipPath->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   }
   else
   {
      m_ClipPath = static_cast<TvgRenderPath*>(path)->tvgShape.get();
      m_ClipPath->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   }
}

namespace rive
{
   RenderPath* makeRenderPath() { return new TvgRenderPath();}
   RenderPaint* makeRenderPaint() { return new TvgRenderPaint();}
}