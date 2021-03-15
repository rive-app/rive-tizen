#include "thorvg_renderer.hpp"
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

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   auto m_PathType = static_cast<TvgRenderPath*>(path)->m_PathType;
   auto m_PathPoints = static_cast<TvgRenderPath*>(path)->m_PathPoints;
   int index = 0;

   /* OPTIMIZE ME: Should avoid data copy in loop... */

   for (size_t i = 0; i < m_PathType.size(); i++)
   {
      /* OPTIMIZE ME: apply transform only when it's not identity */
      switch(m_PathType[i])
      {
         case PathCommand::MoveTo:
         {
            auto pt = applyTransform(m_PathPoints[index], transform);
            m_Shape->moveTo(pt.x, pt.y);
            index += 1;
            break;
         }
         case PathCommand::LineTo:
         {
            auto pt = applyTransform(m_PathPoints[index], transform);
            m_Shape->lineTo(pt.x, pt.y);
            index += 1;
            break;
         }
         case PathCommand::CubicTo:
         {
            auto pt1 = applyTransform(m_PathPoints[index], transform);
            auto pt2 = applyTransform(m_PathPoints[index + 1], transform);
            auto pt3 = applyTransform(m_PathPoints[index + 2], transform);
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

void TvgRenderPath::reset()
{
   m_Shape->reset();
   m_PathType.clear();
   m_PathPoints.clear();
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

void TvgRenderer::save()
{
}

void TvgRenderer::restore()
{
}

void TvgRenderer::transform(const Mat2D& transform)
{
   m_Transform = transform;
}

void TvgRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
   auto renderPath = static_cast<TvgRenderPath*>(path);
   auto shape = static_cast<TvgRenderPath*>(path)->shape();
   auto tvgPaint = static_cast<TvgRenderPaint*>(paint)->paint();

   /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
      thorvg doesn't need to handle both, we can avoid one of them rendering... */

   if (tvgPaint->style == RenderPaintStyle::fill)
   {
      shape->fill(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
   }
   else if (tvgPaint->style == RenderPaintStyle::stroke)
   {
      shape->stroke(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      shape->stroke(tvgPaint->cap);
      shape->stroke(tvgPaint->join);
      shape->stroke(tvgPaint->thickness);
   }

   shape->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});

   if (renderPath->onCanvas())
   {
      m_Canvas->update(shape);
   }
   else
   {
      m_Canvas->push(unique_ptr<Shape>(shape));
      renderPath->onCanvas(true);
   }
}

void TvgRenderer::clipPath(RenderPath* path)
{

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

void TvgRadialGradientBuilder::make(TvgPaint* paint)
{
   /* Implements Tvg Radial Gradient Here*/
   int numStops = stops.size();
   if (numStops != 0)
   {
      unsigned int value = stops[0].color;
      paint->color[0] = value >> 16 & 255;
      paint->color[1] = value >> 8 & 255;
      paint->color[2] = value >> 0 & 255;
      paint->color[3] = value >> 24 & 255;
   }
}

void TvgLinearGradientBuilder::make(TvgPaint* paint)
{
   /* Implements Tvg Linear Gradient Here*/
   int numStops = stops.size();

   if (numStops != 0)
   {
      unsigned int value = stops[0].color;
      paint->color[0] = value >> 16 & 255;
      paint->color[1] = value >> 8 & 255;
      paint->color[2] = value >> 0 & 255;
      paint->color[3] = value >> 24 & 255;
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

namespace rive
{
   RenderPath* makeRenderPath() { return new TvgRenderPath();}
   RenderPaint* makeRenderPaint() { return new TvgRenderPaint();}
}
