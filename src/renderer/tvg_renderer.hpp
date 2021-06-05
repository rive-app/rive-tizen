#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include "file.hpp"
#include "node.hpp"
#include "shapes/paint/fill.hpp"
#include "shapes/paint/stroke.hpp"
#include "shapes/paint/color.hpp"
#include "shapes/paint/solid_color.hpp"
#include "artboard.hpp"
#include "animation/linear_animation.hpp"
#include "animation/linear_animation_instance.hpp"
#include "renderer.hpp"

using namespace tvg;
using namespace std;

#ifndef RIVE_EXPORT
#define RIVE_EXPORT __attribute__((visibility("default")))
#endif

namespace rive
{
   class RIVE_EXPORT TvgFill : public Fill
   {
   public:
      TvgFill(){}
   };

   class RIVE_EXPORT TvgStroke : public Stroke
   {
   public:
      TvgStroke(){}
   };

   class RIVE_EXPORT TvgSolidColor : public SolidColor
   {
   public:
      TvgSolidColor(){}
   };

   class RIVE_EXPORT TvgArtboard : public Artboard
   {
   public:
      TvgArtboard(){}
   };

   class RIVE_EXPORT TvgFile
   {
      rive::File* file;
   public:
      TvgFile(){}
      TvgArtboard* readFile(uint8_t* bytes, size_t length)
      {
        auto reader = rive::BinaryReader(bytes, length);
        auto result = rive::File::import(reader, &file);
        if (result != rive::ImportResult::success)
        {
           return nullptr;
        }

        return (rive::TvgArtboard*)file->artboard();
      }
   };

   class RIVE_EXPORT TvgLinearAnimation : LinearAnimation
   {
   public:
      TvgLinearAnimation(){}
   };

   class RIVE_EXPORT TvgLinearAnimationInstance : public LinearAnimationInstance
   {
   public:
      TvgLinearAnimationInstance(const TvgLinearAnimation* animation):LinearAnimationInstance((LinearAnimation*)animation){}
   };

   struct RIVE_EXPORT TvgPaint
   {
      uint8_t color[4];
      float thickness = 1.0f;
      tvg::Fill *gradientFill = nullptr;
      tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
      tvg::StrokeCap  cap = tvg::StrokeCap::Butt;
      RenderPaintStyle style = RenderPaintStyle::fill;
      bool isGradient = false;
   };

   struct RIVE_EXPORT TvgRenderPath : public RenderPath
   {
      unique_ptr<Shape> tvgShape;

      TvgRenderPath() : tvgShape(tvg::Shape::gen()) {}

      void buildShape();
      void reset() override;
      void addRenderPath(RenderPath* path, const Mat2D& transform) override;
      void fillRule(FillRule value) override;
      void moveTo(float x, float y) override;
      void lineTo(float x, float y) override;
      void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;
      void close() override;
   };

   struct RIVE_EXPORT GradientStop
   {
      unsigned int color;
      float stop;
      GradientStop(unsigned int color, float stop) : color(color), stop(stop)
      {
      }
   };

   class RIVE_EXPORT TvgGradientBuilder
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

   class RIVE_EXPORT TvgRadialGradientBuilder : public TvgGradientBuilder
   {
   public:
      TvgRadialGradientBuilder(float sx, float sy, float ex, float ey) :
          TvgGradientBuilder(sx, sy, ex, ey)
      {
      }
      void make(TvgPaint* paint) override;
   };

   class RIVE_EXPORT TvgLinearGradientBuilder : public TvgGradientBuilder
   {
   public:
      TvgLinearGradientBuilder(float sx, float sy, float ex, float ey) :
          TvgGradientBuilder(sx, sy, ex, ey)
      {
      }
      void make(TvgPaint* paint) override;
   };

   class RIVE_EXPORT TvgRenderPaint : public RenderPaint
   {
   private:
      TvgPaint m_Paint;
      TvgGradientBuilder* m_GradientBuilder = nullptr;

   public:
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

   class RIVE_EXPORT TvgRenderer : public Renderer
   {
   private:
      Canvas* m_Canvas;
      Shape* m_ClipPath = nullptr;
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
}

#endif
