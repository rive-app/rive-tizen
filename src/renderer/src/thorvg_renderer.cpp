#include "thorvg_renderer.hpp"
#include "math/vec2d.hpp"
#include "shapes/paint/color.hpp"

using namespace rive;

TvgRenderPath::TvgRenderPath()
{
   m_Shape = Shape::gen().release();
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

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   auto tvgRenderPath = reinterpret_cast<TvgRenderPath*>(path);
   const PathCommand* cmds = nullptr;
   const Point* pts = nullptr;
   auto cmdCnt = tvgRenderPath->shape()->pathCommands(&cmds);
   auto ptsCnt = tvgRenderPath->shape()->pathCoords(&pts);
   m_Shape->appendPath(cmds, cmdCnt, pts, ptsCnt);
}

void TvgRenderPath::reset()
{
   m_Shape->reset();
}

void TvgRenderPath::moveTo(float x, float y)
{
   m_Shape->moveTo(x, y);
}

void TvgRenderPath::lineTo(float x, float y)
{
   m_Shape->lineTo(x, y);
}

void TvgRenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
   m_Shape->cubicTo(ox, oy, ix, iy, x, y);
}

void TvgRenderPath::close()
{
   m_Shape->close();
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
   auto shape = static_cast<TvgRenderPath*>(path)->shape();
   auto tvgPaint = static_cast<TvgRenderPaint*>(paint)->paint();

   /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
      thorvg doesn't need to handle both, we can avoid one of them rendering... */

   if (tvgPaint->style ==  RenderPaintStyle::fill)
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

   shape->transform({m_Transform[0], 0, m_Transform[4], 0, m_Transform[3], m_Transform[5], 0, 0, 1});

   if (static_cast<TvgRenderPath*>(path)->onCanvas())
   {
      m_Canvas->update(shape);
   }
   else
   {
      m_Canvas->push(unique_ptr<Shape>(shape));
      static_cast<TvgRenderPath*>(path)->onCanvas(true);
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
   m_Paint.color[0] = static_cast<uint8_t>(value >> 16 & 255);
   m_Paint.color[1] = static_cast<uint8_t>(value >> 8 & 255);
   m_Paint.color[2] = static_cast<uint8_t>(value >> 0 & 255);
   m_Paint.color[3] = static_cast<uint8_t>(value >> 24 & 255);
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

}

void TvgRenderPaint::radialGradient(float sx, float sy, float ex, float ey)
{
}

void TvgRenderPaint::addStop(unsigned int color, float stop)
{
}

void TvgRenderPaint::completeGradient()
{

}

void TvgRenderPaint::blendMode(BlendMode value)
{

}

namespace rive
{
   RenderPath* makeRenderPath() { return new TvgRenderPath();}
   RenderPaint* makeRenderPaint() { return new TvgRenderPaint();}
}