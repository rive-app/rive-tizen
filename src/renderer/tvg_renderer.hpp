#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include <stack>
#include "rive/renderer.hpp"

using namespace tvg;
using namespace std;

namespace rive
{
   struct TvgPaint
   {
      uint8_t color[4];
      float thickness = 1.0f;
      tvg::Fill *gradientFill = nullptr;
      tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
      tvg::StrokeCap  cap = tvg::StrokeCap::Butt;
      RenderPaintStyle style = RenderPaintStyle::fill;
      bool isGradient = false;
   };

   class TvgRenderPath : public RenderPath
   {
   private:
      unique_ptr<Shape> m_Path;
   public:
      TvgRenderPath() : m_Path(tvg::Shape::gen()) {}

      Shape* path() const { return m_Path.get(); }
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
      TvgPaint* paint() { return &m_Paint; }
      void style(RenderPaintStyle style) override;
      void color(unsigned int value) override;
      void thickness(float value) override;
      void join(StrokeJoin value) override;
      void cap(StrokeCap value) override;
      void blendMode(BlendMode value) override;
      void shader(rcp<RenderShader>) override;
   };

   class TvgRenderImage : public RenderImage {
   private:
      unique_ptr<Picture> m_Image;
   public:
      Picture* image() const { return m_Image.get(); };
      bool decode(Span<const uint8_t>) override;
      rcp<RenderShader> makeShader(RenderTileMode tx, RenderTileMode ty, const Mat2D* localMatrix) const override;
   };

   class TvgRenderer : public Renderer
   {
   protected:
      Canvas* m_Canvas;
      Shape* m_ClipPath = nullptr;
      Shape* m_BgClipPath = nullptr;
      Mat2D m_Transform;
      stack<Mat2D> m_SavedTransforms;
   public:
      TvgRenderer(Canvas* canvas) : m_Canvas(canvas) {}
      void save() override;
      void restore() override;
      void transform(const Mat2D& transform) override;
      void clipPath(RenderPath* path) override;
      void drawPath(RenderPath* path, RenderPaint* paint) override;
      void drawImage(const RenderImage*, BlendMode, float opacity) override;
      void drawImageMesh(const RenderImage*, rcp<RenderBuffer> vertices_f32, rcp<RenderBuffer> uvCoords_f32, rcp<RenderBuffer> indices_u16, BlendMode, float opacity) override;
   };

} // namespace

#endif
