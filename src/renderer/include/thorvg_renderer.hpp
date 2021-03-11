#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include "renderer.hpp"

using namespace tvg;
using namespace std;

namespace rive
{
   struct TvgPoint
   {
      float x, y;
   };

   struct TvgPaint
   {
      int fillColor[4];
      int strokeColor[4];
      float thickness;
      tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
      tvg::StrokeCap  cap = tvg::StrokeCap::Butt;
      RenderPaintStyle style;
      bool isFill = false;
      bool isStroke = false;
   };

   class TvgRenderPath : public RenderPath
   {
   private:
      Shape *m_Shape;
      vector<PathCommand> m_PathType;
      vector<TvgPoint> m_PathPoints;
      bool m_Pushed = false;

   public:
      TvgRenderPath();
      Shape* shape() { return m_Shape; }
      bool getPushed() { return m_Pushed; }
      void setPushed(bool pushed) { m_Pushed = pushed; }
      void reset() override;
      void addRenderPath(RenderPath* path, const Mat2D& transform) override;
      void fillRule(FillRule value) override;
      void moveTo(float x, float y) override;
      void lineTo(float x, float y) override;
      void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;
      virtual void close() override;
   };

   class TvgRenderPaint : public RenderPaint
   {
   private:
      TvgPaint m_Paint;

   public:
      TvgRenderPaint();
      TvgPaint* paint() { return &m_Paint; }
      void style(RenderPaintStyle style) override;
      void color(unsigned int value) override;
      void thickness(float value) override;
      void join(StrokeJoin value) override;
      void cap(StrokeCap value) override;
      void blendMode(BlendMode value) override;

      void linearGradient(float sx, float sy, float ex, float ey) override;
      void radialGradient(float sx, float sy, float ex, float ey) override;
      void addStop(unsigned int color, float stop) override;
      void completeGradient() override;
   };

   class TvgRenderer : public Renderer
   {
   private:
      Canvas* m_Canvas;
      Mat2D m_Transform;

   public:
      TvgRenderer(Canvas* canvas) : m_Canvas(canvas) {}
      void save() override;
      void restore() override;
      void transform(const Mat2D& transform) override;
      void drawPath(RenderPath* path, RenderPaint* paint) override;
      void clipPath(RenderPath* path) override;
   };
} // namespace rive
#endif
