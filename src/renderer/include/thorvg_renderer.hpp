#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include "renderer.hpp"

namespace rive
{
   enum ThorvgPathType 
   {
      MoveTo,
      LineTo,
      CubicTo,
      Close
   };

   struct ThorvgPoint
   {
      float x, y;
      ThorvgPoint(int _x, int _y) : x(_x), y(_y) { }
   };

   struct ThorvgPaint
   {
      int r, g, b, a;
      float thickness;
      ThorvgPaint() : r(0), g(0), b(0), a(0), thickness(0.0) {}
   };

   class ThorvgRenderPath : public RenderPath
   {
   private:
      tvg::Shape *m_Path;
      std::vector<ThorvgPathType> m_PathType;
      std::vector<ThorvgPoint> m_PathPoints;
      bool pushed;

   public:
      ThorvgRenderPath();
      tvg::Shape* path() { return m_Path; }
      void reset() override;
      void addRenderPath(RenderPath* path, const Mat2D& transform) override;
      void fillRule(FillRule value) override;
      void moveTo(float x, float y) override;
      void lineTo(float x, float y) override;
      void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;
      virtual void close() override;
   };

   class ThorvgRenderPaint : public RenderPaint
   {
   private:
      ThorvgPaint m_Paint;

   public:
      ThorvgRenderPaint();
      ThorvgPaint* paint() { return &m_Paint; }
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

   class ThorvgRenderer : public Renderer
   {
   private:
      tvg::Canvas* m_Canvas;
      rive::Mat2D m_Transform;

   public:
      ThorvgRenderer(tvg::Canvas* canvas) : m_Canvas(canvas) {}
      void save() override;
      void restore() override;
      void transform(const Mat2D& transform) override;
      void drawPath(RenderPath* path, RenderPaint* paint) override;
      void clipPath(RenderPath* path) override;
   };
} // namespace rive
#endif
