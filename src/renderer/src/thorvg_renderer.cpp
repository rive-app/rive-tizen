#include "thorvg_renderer.hpp"
#include "math/vec2d.hpp"
#include "shapes/paint/color.hpp"

using namespace rive;

TvgRenderPath::TvgRenderPath()
{
	this->m_Path = tvg::Shape::gen().release();
}

void TvgRenderPath::fillRule(FillRule value)
{
	switch (value)
	{
		case FillRule::evenOdd:
			m_Path->fill(tvg::FillRule::EvenOdd);
			break;
		case FillRule::nonZero:
			break;
	}
}

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   auto m_PathType = reinterpret_cast<TvgRenderPath*>(path)->m_PathType;
   auto m_PathPoints = reinterpret_cast<TvgRenderPath*>(path)->m_PathPoints;

   int index = 0;
   for (int i = 0; i < m_PathType.size(); i++)
   {
      PathCommand type = m_PathType[i];
      switch(type)
      {
         case PathCommand::MoveTo:
            m_Path->moveTo(m_PathPoints[index].x, m_PathPoints[index].y);
            index += 1;
            break;
         case PathCommand::LineTo:
            m_Path->lineTo(m_PathPoints[index].x, m_PathPoints[index].y);
            index += 1;
            break;
         case PathCommand::CubicTo:
            m_Path->cubicTo(m_PathPoints[index].x, m_PathPoints[index].y,
                            m_PathPoints[index+1].x, m_PathPoints[index+1].y,
                            m_PathPoints[index+2].x, m_PathPoints[index+2].y);
            index += 3;
            break;
         case PathCommand::Close:
            m_Path->close();
            break;
      }
   }
}

void TvgRenderPath::reset()
{
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
	Matrix m = {m_Transform[0], 0, m_Transform[4],
                    0, m_Transform[3], m_Transform[5],
                    0, 0, 1};

   TvgRenderPath *renderPath = reinterpret_cast<TvgRenderPath*>(path);
   Shape *drawPath = reinterpret_cast<TvgRenderPath*>(path)->path();
   drawPath->transform(m);

   TvgPaint *drawPaint = reinterpret_cast<TvgRenderPaint*>(paint)->paint();
   if (drawPaint->isFill)
     drawPath->fill(drawPaint->fillColor[0], drawPaint->fillColor[1], drawPaint->fillColor[2], drawPaint->fillColor[3]);
   if (drawPaint->isStroke)
   {
     drawPath->stroke(drawPaint->strokeColor[0], drawPaint->strokeColor[1], drawPaint->strokeColor[2], drawPaint->strokeColor[3]);
     drawPath->stroke(drawPaint->cap);
     drawPath->stroke(drawPaint->join);
     if (drawPaint->thickness != 0)
       drawPath->stroke(drawPaint->thickness);
   }

   if (!renderPath->getPushed())
   {
      Result ret = m_Canvas->push(unique_ptr<Shape>(drawPath));
      renderPath->setPushed(true);
   }
   else
   {
      Result ret = m_Canvas->update(drawPath);
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
	switch (style)
	{
		case RenderPaintStyle::fill:
         m_Paint.style = RenderPaintStyle::fill;
         m_Paint.isFill = true;
			break;
		case RenderPaintStyle::stroke:
         m_Paint.style = RenderPaintStyle::stroke;
         m_Paint.isStroke = true;
			break;
	}
}
void TvgRenderPaint::color(unsigned int value)
{
   int b = value >> 0 & 255;
   int g = value >> 8 & 255;
   int r = value >> 16 & 255;
   int a = value >> 24 & 255;

   if (m_Paint.style == RenderPaintStyle::fill)
   {
      m_Paint.fillColor[0] = r;
      m_Paint.fillColor[1] = g;
      m_Paint.fillColor[2] = b;
      m_Paint.fillColor[3] = a;
   }
   else if (m_Paint.style == RenderPaintStyle::stroke)
   {
      m_Paint.strokeColor[0] = r;
      m_Paint.strokeColor[1] = g;
      m_Paint.strokeColor[2] = b;
      m_Paint.strokeColor[3] = a;
   }
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