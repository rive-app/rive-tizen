#include "thorvg_renderer.hpp"
#include "math/vec2d.hpp"
#include "shapes/paint/color.hpp"

using namespace rive;

TvgRenderPath::TvgRenderPath()
{
	this->m_Shape = tvg::Shape::gen().release();
}

void TvgRenderPath::fillRule(FillRule value)
{
	switch (value)
	{
		case FillRule::evenOdd:
			m_Shape->fill(tvg::FillRule::EvenOdd);
			break;
		case FillRule::nonZero:
			break;
	}
}

Vec2D applyTransform(const Vec2D &vec, const Mat2D &mat)
{
	tvg::Matrix m = {1, 0, 0, 0, 1, 0, 0, 0, 1};
	m.e11 = mat[0];
	m.e12 = mat[2];
	m.e13 = mat[4];
	m.e21 = mat[1];
	m.e22 = mat[3];
	m.e23 = mat[5];

	Vec2D ret;
	ret[0] = round(vec[0] * m.e11 + vec[1] * m.e12 + m.e13);
	ret[1] = round(vec[0] * m.e21 + vec[1] * m.e22 + m.e23);

	return ret;
}

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   auto m_PathType = reinterpret_cast<TvgRenderPath*>(path)->m_PathType;
   auto m_PathPoints = reinterpret_cast<TvgRenderPath*>(path)->m_PathPoints;
   Vec2D vec1, vec2, vec3, vecOut1, vecOut2, vecOut3;

   int index = 0;
   for (size_t i = 0; i < m_PathType.size(); i++)
   {
      PathCommand type = m_PathType[i];
      switch(type)
      {
         case PathCommand::MoveTo:
            vecOut1 = applyTransform(m_PathPoints[index], transform);
            m_Shape->moveTo(vecOut1[0], vecOut1[1]);
            index += 1;
            break;
         case PathCommand::LineTo:
            vecOut1 = applyTransform(m_PathPoints[index], transform);
            m_Shape->lineTo(vecOut1[0], vecOut1[1]);
            index += 1;
            break;
         case PathCommand::CubicTo:
            vecOut1 = applyTransform(m_PathPoints[index], transform);
            vecOut2 = applyTransform(m_PathPoints[index + 1], transform);
            vecOut3 = applyTransform(m_PathPoints[index + 2], transform);
            m_Shape->cubicTo(vecOut1[0], vecOut1[1],
                             vecOut2[0], vecOut2[1],
                             vecOut3[0], vecOut3[1]);
            index += 3;
            break;
         case PathCommand::Close:
            m_Shape->close();
            break;
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
	Matrix m = {m_Transform[0], 0, m_Transform[4], 0, m_Transform[3], m_Transform[5], 0, 0, 1};

   auto renderPath = reinterpret_cast<TvgRenderPath*>(path);
   auto shape = reinterpret_cast<TvgRenderPath*>(path)->shape();
   shape->transform(m);

   auto tvgPaint = reinterpret_cast<TvgRenderPaint*>(paint)->paint();

   if (tvgPaint->isFill)
   {
      shape->fill(tvgPaint->fillColor[0], tvgPaint->fillColor[1], tvgPaint->fillColor[2], tvgPaint->fillColor[3]);
   }

   if (tvgPaint->isStroke)
   {
      shape->stroke(tvgPaint->strokeColor[0], tvgPaint->strokeColor[1], tvgPaint->strokeColor[2], tvgPaint->strokeColor[3]);
      shape->stroke(tvgPaint->cap);
      shape->stroke(tvgPaint->join);
      if (tvgPaint->thickness != 0)
      {
         shape->stroke(tvgPaint->thickness);
      }
   }

   if (!renderPath->getPushed())
   {
      m_Canvas->push(unique_ptr<Shape>(shape));
      renderPath->setPushed(true);
   }
   else
   {
      m_Canvas->update(shape);
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
