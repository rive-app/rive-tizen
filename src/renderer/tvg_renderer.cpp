#include "tvg_renderer.hpp"
#include "math/vec2d.hpp"
#include "shapes/paint/color.hpp"

using namespace rive;

TvgRenderPath::TvgRenderPath()
{
   this->m_Shape = tvg::Shape::gen().release();
}

TvgRenderPath::~TvgRenderPath()
{
   if (!active) delete(m_Shape);
}

void TvgRenderPath::fillRule(FillRule value)
{
   switch (value)
   {
      case FillRule::evenOdd:
         m_Shape->fill(tvg::FillRule::EvenOdd);
         break;
      case FillRule::nonZero:
         m_Shape->fill(tvg::FillRule::Winding);
         break;
   }
}

Point applyTransform(const Vec2D &vec, const Mat2D &mat)
{
   Matrix m = {1, 0, 0, 0, 1, 0, 0, 0, 1};
   m.e11 = mat[0];
   m.e12 = mat[2];
   m.e13 = mat[4];
   m.e21 = mat[1];
   m.e22 = mat[3];
   m.e23 = mat[5];

   return {vec[0] * m.e11 + vec[1] * m.e12 + m.e13, vec[0] * m.e21 + vec[1] * m.e22 + m.e23};
}

void TvgRenderPath::reset()
{
   m_Shape->reset();
   m_PathType.clear();
   m_PathPoints.clear();
}

void TvgRenderPath::buildShape()
{
   int index = 0;
   for (size_t i = 0; i < m_PathType.size(); i++)
   {
      switch(m_PathType[i])
      {
         case PathCommand::MoveTo:
         {
            m_Shape->moveTo(m_PathPoints[index][0], m_PathPoints[index][1]);
            index += 1;
            break;
         }
         case PathCommand::LineTo:
         {
            m_Shape->lineTo(m_PathPoints[index][0], m_PathPoints[index][1]);
            index += 1;
            break;
         }
         case PathCommand::CubicTo:
         {
            m_Shape->cubicTo(m_PathPoints[index][0], m_PathPoints[index][1],
                             m_PathPoints[index + 1][0], m_PathPoints[index + 1][1],
                             m_PathPoints[index + 2][0], m_PathPoints[index + 2][1]);
            index += 3;
            break;
         }
         case PathCommand::Close:
         {
            m_Shape->close();
            index += 1;
            break;
         }
      }
   }

   m_PathType.clear();
   m_PathPoints.clear();
}

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   m_PathType = static_cast<TvgRenderPath*>(path)->m_PathType;
   /* OPTIMIZE ME: Should avoid data copy here */
   auto srcPathPoints = static_cast<TvgRenderPath*>(path)->m_PathPoints;
   m_PathPoints.resize(srcPathPoints.size());
   std::copy(srcPathPoints.begin(), srcPathPoints.end(), m_PathPoints.begin());

   /* OPTIMIZE ME: Should avoid data copy in loop... */
   int index = 0;

   if (m_PathType.size() != 0)
     shapeAdded = true;

   for (size_t i = 0; i < m_PathType.size(); i++)
   {
      /* OPTIMIZE ME: apply transform only when it's not identity */
      switch(m_PathType[i])
      {
         case PathCommand::MoveTo:
         {
            auto pt = applyTransform(m_PathPoints[index], transform);
            m_PathPoints[index] = {pt.x, pt.y};
            m_Shape->moveTo(pt.x, pt.y);
            index += 1;
            break;
         }
         case PathCommand::LineTo:
         {
            auto pt = applyTransform(m_PathPoints[index], transform);
            m_PathPoints[index] = {pt.x, pt.y};
            m_Shape->lineTo(pt.x, pt.y);
            index += 1;
            break;
         }
         case PathCommand::CubicTo:
         {
            auto pt1 = applyTransform(m_PathPoints[index], transform);
            auto pt2 = applyTransform(m_PathPoints[index + 1], transform);
            auto pt3 = applyTransform(m_PathPoints[index + 2], transform);
            m_PathPoints[index] = {pt1.x, pt1.y};
            m_PathPoints[index + 1] = {pt2.x, pt2.y};
            m_PathPoints[index + 2] = {pt3.x, pt3.y};
            m_Shape->cubicTo(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y);
            index += 3;
            break;
         }
         case PathCommand::Close:
         {
            m_Shape->close();
            index += 1;
            break;
         }
      }
   }
}

void TvgRenderPath::moveTo(float x, float y)
{
   m_PathType.push_back(PathCommand::MoveTo);
   m_PathPoints.push_back({x, y});
}

void TvgRenderPath::lineTo(float x, float y)
{
   m_PathType.push_back(PathCommand::LineTo);
   m_PathPoints.push_back({x, y});
}

void TvgRenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
   m_PathType.push_back(PathCommand::CubicTo);
   m_PathPoints.push_back({ox, oy});
   m_PathPoints.push_back({ix, iy});
   m_PathPoints.push_back({x, y});
}

void TvgRenderPath::close()
{
   m_PathType.push_back(PathCommand::Close);
}

TvgRenderPaint::TvgRenderPaint()
{

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
   m_SaveTransform = m_Transform;
}

void TvgRenderer::restore()
{
   m_Transform = m_SaveTransform;
}

void TvgRenderer::transform(const Mat2D& transform)
{
   m_Transform = m_Transform * transform;
}

void TvgRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
   auto renderPath = static_cast<TvgRenderPath*>(path);
   auto shape = static_cast<TvgRenderPath*>(path)->shape();
   auto tvgPaint = static_cast<TvgRenderPaint*>(paint)->paint();

   /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
      thorvg doesn't need to handle both, we can avoid one of them rendering... */
   if (!renderPath->isShapeAdded())
     renderPath->buildShape();

   if (tvgPaint->style == RenderPaintStyle::fill)
   {
      if (tvgPaint->isGradient == false)
         shape->fill(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      else
      {
         if (tvgPaint->gradientApplied == false)
         {
            shape->fill(unique_ptr<tvg::Fill>(tvgPaint->gradientFill));
            tvgPaint->gradientApplied = true;
         }
      }
   }
   else if (tvgPaint->style == RenderPaintStyle::stroke)
   {
      shape->stroke(tvgPaint->cap);
      shape->stroke(tvgPaint->join);
      shape->stroke(tvgPaint->thickness);

      if (tvgPaint->isGradient == false)
         shape->stroke(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      else
      {
        if (tvgPaint->gradientApplied == false)
        {
          shape->stroke(unique_ptr<tvg::Fill>(tvgPaint->gradientFill));
          tvgPaint->gradientApplied = true;
        }
      }
   }

   if (m_ClipPath)
   {
      m_ClipPath->fill(255, 255, 255, 255);
      shape->composite(unique_ptr<Shape>(static_cast<Shape*>(m_ClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      m_ClipPath = nullptr;
   }

   shape->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   m_Canvas->push(unique_ptr<Paint>(shape->duplicate()));
}


void TvgRenderer::clipPath(RenderPath* path)
{
   //Note: ClipPath transform matrix is calculated by transfrom matrix in addRenderPath function
   m_ClipPath = static_cast<TvgRenderPath*>(path)->shape();
   m_ClipPath->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
}

namespace rive
{
   RenderPath* makeRenderPath() { return new TvgRenderPath();}
   RenderPaint* makeRenderPaint() { return new TvgRenderPaint();}
}
