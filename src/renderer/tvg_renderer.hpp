#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include "renderer.hpp"

using namespace tvg;
using namespace std;

namespace rive
{
   struct TvgPaint
   {
      uint8_t color[4];
      float thickness;
      tvg::Fill *gradientFill;
      tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
      tvg::StrokeCap  cap = tvg::StrokeCap::Butt;
      RenderPaintStyle style;
      bool isGradient;
      bool gradientApplied;
      TvgPaint() : isGradient(false), gradientApplied(false) {}
   };

   class TvgRenderPath : public RenderPath
   {
   private:
      Shape *m_Shape;
      vector<PathCommand> m_PathType;
      vector<Vec2D> m_PathPoints;

   public:
      TvgRenderPath();
      ~TvgRenderPath();
      Shape* shape() { return m_Shape; }
      void buildShape();
      void reset() override;
      void addRenderPath(RenderPath* path, const Mat2D& transform) override;
      void fillRule(FillRule value) override;
      void moveTo(float x, float y) override;
      void lineTo(float x, float y) override;
      void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;
      virtual void close() override;
   };

   struct GradientStop
   {
      unsigned int color;
      float stop;
      GradientStop(unsigned int color, float stop) : color(color), stop(stop)
      {
      }
   };

   class TvgGradientBuilder
   {
   public:
      std::vector<GradientStop> stops;
      float sx, sy, ex, ey;
      virtual ~TvgGradientBuilder() {}
      TvgGradientBuilder(float sx, float sy, float ex, float ey) :
          sx(sx), sy(sy), ex(ex), ey(ey)
      {
      }

      virtual void make(TvgPaint* paint) = 0;
   };

   class TvgRadialGradientBuilder : public TvgGradientBuilder
   {
   public:
      TvgRadialGradientBuilder(float sx, float sy, float ex, float ey) :
          TvgGradientBuilder(sx, sy, ex, ey)
      {
      }
      void make(TvgPaint* paint) override;
   };

   class TvgLinearGradientBuilder : public TvgGradientBuilder
   {
   public:
      TvgLinearGradientBuilder(float sx, float sy, float ex, float ey) :
          TvgGradientBuilder(sx, sy, ex, ey)
      {
      }
      void make(TvgPaint* paint) override;
   };

   class TvgRenderPaint : public RenderPaint
   {
   private:
      TvgPaint m_Paint;
      TvgGradientBuilder* m_GradientBuilder;

   public:
      TvgPaint* paint() { return &m_Paint; }
      TvgRenderPaint();
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
      Shape* m_ClipPath;
      Mat2D m_Transform;
      Mat2D m_SaveTransform;

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
