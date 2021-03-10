#include "thorvg_renderer.hpp"
#include "math/vec2d.hpp"
#include "shapes/paint/color.hpp"

using namespace rive;

ThorvgRenderPath::ThorvgRenderPath() : pushed(false)
{
	this->m_Path = tvg::Shape::gen().release();
}

void ThorvgRenderPath::fillRule(FillRule value)
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

void ThorvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   std::vector<ThorvgPathType> &m_PathType = reinterpret_cast<ThorvgRenderPath*>(path)->m_PathType;
   std::vector<ThorvgPoint> &m_PathPoints = reinterpret_cast<ThorvgRenderPath*>(path)->m_PathPoints;

   int index = 0;
   for (int i = 0; i < m_PathType.size(); i++)
   {
      ThorvgPathType type = m_PathType[i];
      switch(type)
      {
         case ThorvgPathType::MoveTo:
            m_Path->moveTo(m_PathPoints[index].x, m_PathPoints[index].y);
            index += 1;
            break;
         case ThorvgPathType::LineTo:
            m_Path->moveTo(m_PathPoints[index].x, m_PathPoints[index].y);
            index += 1;
            break;
         case ThorvgPathType::CubicTo:
            m_Path->cubicTo(m_PathPoints[index].x, m_PathPoints[index].y,
                            m_PathPoints[index+1].x, m_PathPoints[index+1].y,
                            m_PathPoints[index+2].x, m_PathPoints[index+2].y);
            index += 3;
            break;
         case ThorvgPathType::Close:
            m_Path->close();
            break;
      }
   }
}

void ThorvgRenderPath::reset()
{
   m_PathType.clear();
   m_PathPoints.clear();
}

void ThorvgRenderPath::moveTo(float x, float y)
{
   m_PathType.push_back(ThorvgPathType::MoveTo);
   m_PathPoints.push_back(ThorvgPoint(x, y));
}

void ThorvgRenderPath::lineTo(float x, float y)
{
   m_PathType.push_back(ThorvgPathType::LineTo);
   m_PathPoints.push_back(ThorvgPoint(x, y));
}

void ThorvgRenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
   m_PathType.push_back(ThorvgPathType::CubicTo);
   m_PathPoints.push_back(ThorvgPoint(ox, oy));
   m_PathPoints.push_back(ThorvgPoint(ix, iy));
   m_PathPoints.push_back(ThorvgPoint(x, y));
}

void ThorvgRenderPath::close()
{
   m_PathType.push_back(ThorvgPathType::Close);
}

void ThorvgRenderer::save()
{
}

void ThorvgRenderer::restore()
{
}

void ThorvgRenderer::transform(const Mat2D& transform)
{
   m_Transform = transform;
}

void ThorvgRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
	tvg::Matrix m = {m_Transform[0], 0, m_Transform[4],
                    0, m_Transform[3], m_Transform[5],
                    0, 0, 1};

   tvg::Shape *drawPath = reinterpret_cast<ThorvgRenderPath*>(path)->path();
	drawPath->transform(m);

   ThorvgPaint *drawPaint = reinterpret_cast<ThorvgRenderPaint*>(paint)->paint();
   drawPath->fill(drawPaint->r, drawPaint->g, drawPaint->b, drawPaint->a);
   drawPath->stroke(drawPaint->thickness);

   m_Canvas->push(std::unique_ptr<tvg::Shape>(drawPath));
}

void ThorvgRenderer::clipPath(RenderPath* path)
{

}

ThorvgRenderPaint::ThorvgRenderPaint()
{

}

void ThorvgRenderPaint::style(RenderPaintStyle style)
{
	switch (style)
	{
		case RenderPaintStyle::fill:
			break;
		case RenderPaintStyle::stroke:
			break;
	}
}
void ThorvgRenderPaint::color(unsigned int value)
{
   m_Paint.r = value >> 0 & 255;
   m_Paint.g = value >> 8 & 255;
   m_Paint.b = value >> 16 & 255;
   m_Paint.a = value >> 24 & 255;
}

void ThorvgRenderPaint::thickness(float value)
{
   m_Paint.thickness = value;
}

void ThorvgRenderPaint::join(StrokeJoin value)
{

}

void ThorvgRenderPaint::cap(StrokeCap value)
{

}

void ThorvgRenderPaint::linearGradient(float sx, float sy, float ex, float ey)
{

}

void ThorvgRenderPaint::radialGradient(float sx, float sy, float ex, float ey)
{
}

void ThorvgRenderPaint::addStop(unsigned int color, float stop)
{
}

void ThorvgRenderPaint::completeGradient()
{

}

void ThorvgRenderPaint::blendMode(BlendMode value)
{

}

namespace rive
{
   RenderPath* makeRenderPath() { return new ThorvgRenderPath();}
   RenderPaint* makeRenderPaint() { return new ThorvgRenderPaint();}
}
