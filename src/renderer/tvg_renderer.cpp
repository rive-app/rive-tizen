#include "tvg_renderer.hpp"
#include "rive/math/vec2d.hpp"
#include "rive/shapes/paint/color.hpp"

using namespace rive;

void TvgRenderPath::fillRule(FillRule value)
{
   switch (value)
   {
      case FillRule::evenOdd:
         m_Path->fill(tvg::FillRule::EvenOdd);
         break;
      case FillRule::nonZero:
         m_Path->fill(tvg::FillRule::Winding);
         break;
   }
}

Point transformCoord(const Point pt, const Mat2D &transform)
{
   Matrix m = {1, 0, 0, 0, 1, 0, 0, 0, 1};
   m.e11 = transform[0];
   m.e12 = transform[2];
   m.e13 = transform[4];
   m.e21 = transform[1];
   m.e22 = transform[3];
   m.e23 = transform[5];

   return {pt.x * m.e11 + pt.y * m.e12 + m.e13, pt.x * m.e21 + pt.y * m.e22 + m.e23};
}

PathCommand convertCommand(PathVerb verb)
{
   switch (verb)
   {
      case PathVerb::move: return PathCommand::MoveTo;
      case PathVerb::line: return PathCommand::LineTo;
      case PathVerb::cubic: return PathCommand::CubicTo;
      case PathVerb::close: return PathCommand::Close;
      case PathVerb::quad:
      default:
         return PathCommand::Close; // TODOD: What to do here? ThorVG does not support other types
   }
}

class TvgBuffer : public RenderBuffer {
    const size_t m_ElemSize;
    void* m_Buffer;

public:
    TvgBuffer(const void* src, size_t count, size_t elemSize) :
        RenderBuffer(count), m_ElemSize(elemSize) {
        size_t bytes = count * elemSize;
        m_Buffer = malloc(bytes);
        memcpy(m_Buffer, src, bytes);
    }

    ~TvgBuffer() { free(m_Buffer); }

    const float* f32s() const {
        assert(m_ElemSize == sizeof(float));
        return static_cast<const float*>(m_Buffer);
    }

    const uint16_t* u16s() const {
        assert(m_ElemSize == sizeof(uint16_t));
        return static_cast<const uint16_t*>(m_Buffer);
    }

    const Point* points() const { return reinterpret_cast<const Point*>(this->f32s()); }

    static const TvgBuffer* Cast(const RenderBuffer* buffer) {
        return reinterpret_cast<const TvgBuffer*>(buffer);
    }
};

template <typename T> rcp<RenderBuffer> make_buffer(Span<T> span) {
    return rcp<RenderBuffer>(new TvgBuffer(span.data(), span.size(), sizeof(T)));
}

void TvgRenderPath::reset()
{
   m_Path->reset();
}

void TvgRenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
   const Point* pts;
   auto ptsCnt = static_cast<TvgRenderPath*>(path)->m_Path->pathCoords(&pts);
   if (!pts) return;

   const PathCommand* cmds;
   auto cmdCnt = static_cast<TvgRenderPath*>(path)->m_Path->pathCommands(&cmds);
   if (!cmds) return;

   //Capture the last coordinates
   Point* pts2;
   auto ptsCnt2 = m_Path->pathCoords(const_cast<const Point**>(&pts2));

   m_Path->appendPath(cmds, cmdCnt, pts, ptsCnt);

   //Immediate Transform for the newly appended
   Point* pts3;
   auto ptsCnt3 = m_Path->pathCoords(const_cast<const Point**>(&pts3));

   for (unsigned i = ptsCnt2; i < ptsCnt3; ++i)
   {
      pts3[i] = transformCoord(pts3[i], transform);
   }
}

void TvgRenderPath::moveTo(float x, float y)
{
   m_Path->moveTo(x, y);
}

void TvgRenderPath::lineTo(float x, float y)
{
   m_Path->lineTo(x, y);
}

void TvgRenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
   m_Path->cubicTo(ox, oy, ix, iy, x, y);
}

void TvgRenderPath::close()
{
   m_Path->close();
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

void TvgRenderPaint::shader(rcp<RenderShader> shader)
{
   TvgRenderShader* sh = (TvgRenderShader*)shader.release();
   m_Paint.shader = std::unique_ptr<TvgRenderShader>(sh);
}

bool TvgRenderImage::decode(Span<const uint8_t> data)
{
   m_Image = Picture::gen();
   if(m_Image->load((const char *)data.data(), data.size(), "") == tvg::Result::Success)
   {
      if (m_Image->size(&width, &height) == tvg::Result::Success) {
         return true;
      }
      width = height = 0.0;
   }

   return false;
}

rcp<RenderShader> TvgRenderImage::makeShader(RenderTileMode tx, RenderTileMode ty, const Mat2D* localMatrix) const
{
   TvgRenderShader* shader = new TvgRenderShader(m_Image.get(), localMatrix);
   // TODO: Implement tx, ty - probably as properties of the shader. They would map to tvg::FillSpread ?
   //       rive::RenderTileMode = [ clamp, repeat, mirror, decal ], tvg::FillSpread = [ Pad, Reflect, Repeat ]

   return rcp<RenderShader>(shader);
}

void TvgRenderer::save()
{
   TvgRendererState state;
   state.clipPath = m_ClipPath;
   state.transform = m_Transform;
   m_SavedTransforms.push(state);
}

void TvgRenderer::restore()
{
   // Check shouldn't be needed, but safest to check
   if (m_SavedTransforms.size() > 0)
   {
      TvgRendererState state = m_SavedTransforms.top();
      m_SavedTransforms.pop();
      m_ClipPath = state.clipPath;
      m_Transform = state.transform;
   }
}

void TvgRenderer::transform(const Mat2D& transform)
{
   m_Transform = m_Transform * transform;
}

void TvgRenderer::drawPath(RenderPath* path, RenderPaint* paint)
{
   auto tvgShape = static_cast<TvgRenderPath*>(path)->path();
   auto tvgPaint = static_cast<TvgRenderPaint*>(paint)->paint();

   /* OPTIMIZE ME: Stroke / Fill Paints required to draw separately.
      thorvg doesn't need to handle both, we can avoid one of them rendering... */

   if (tvgPaint->style == RenderPaintStyle::fill)
   {
      if (tvgPaint->isPicture())
      {
         // TODO: fill tvgShape with image
         // The image is in tvgPaint->shader->picture()
      }
      else if (tvgPaint->isFill())
      {
         tvgShape->fill(std::unique_ptr<Fill>(tvgPaint->shader->fill()->duplicate()));
      }
      else
      {
         tvgShape->fill(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      }
   }
   else if (tvgPaint->style == RenderPaintStyle::stroke)
   {
      tvgShape->stroke(tvgPaint->cap);
      tvgShape->stroke(tvgPaint->join);
      tvgShape->stroke(tvgPaint->thickness);

      if (tvgPaint->isPicture())
      {
         // TODO: fill tvgShape with image
         // The image is in tvgPaint->shader->picture()
      }
      else if (tvgPaint->isFill())
      {
         tvgShape->stroke(std::unique_ptr<Fill>(tvgPaint->shader->fill()->duplicate()));
      }
      else
      {
         tvgShape->stroke(tvgPaint->color[0], tvgPaint->color[1], tvgPaint->color[2], tvgPaint->color[3]);
      }
   }

   if (m_ClipPath)
   {
      m_ClipPath->fill(255, 255, 255, 255);
      tvgShape->composite(std::unique_ptr<Shape>(static_cast<Shape*>(m_ClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      m_ClipPath = nullptr;
   }

   tvgShape->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});

   if (m_BgClipPath)
   {
      m_BgClipPath->fill(255, 255, 255, 255);
      auto scene = tvg::Scene::gen();
      scene->push(std::unique_ptr<Paint>(tvgShape->duplicate()));
      scene->composite(std::unique_ptr<Shape>(static_cast<Shape*>(m_BgClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      if (m_Canvas) m_Canvas->push(std::move(scene));
      else m_Scene->push(std::move(scene));
   }
   else
   {
      if (m_Canvas) m_Canvas->push(std::unique_ptr<Paint>(tvgShape->duplicate()));
      else m_Scene->push(std::unique_ptr<Paint>(tvgShape->duplicate()));
   }
}

void TvgRenderer::drawImage(const RenderImage* image, BlendMode blendMode, float opacity)
{
   TvgRenderImage* renderImage = (TvgRenderImage*)image;
   float offsetx = -renderImage->width/2;
   float offsety = -renderImage->height/2;

   // Duplicate the image because the RenderImage owns it and re-uses it
   auto paint = renderImage->image()->duplicate();
   opacity = opacity < 0.0 ? 0.0 : opacity > 1.0 ? 1.0 : opacity;
   paint->opacity(int(opacity*255.0));
   //paint->blendMode(blendMode); TODO: Blend mode unsupported by ThorVG

   // ThorVG image handle is top-left, Rive image handle is center. Place image
   // in a scene and translate it to account for this.
   auto paintWrapper = tvg::Scene::gen();
   paint->translate(offsetx, offsety);
   paintWrapper->push(std::unique_ptr<Paint>(paint));

   if (m_ClipPath)
   {
      m_ClipPath->fill(255, 255, 255, 255);
      paintWrapper->composite(std::unique_ptr<Shape>(static_cast<Shape*>(m_ClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      m_ClipPath = nullptr;
   }

   paintWrapper->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   
   if (m_BgClipPath)
   {
      m_BgClipPath->fill(255, 255, 255, 255);
      auto scene = tvg::Scene::gen();
      scene->push(std::move(paintWrapper));
      scene->composite(std::unique_ptr<Shape>(static_cast<Shape*>(m_BgClipPath->duplicate())), tvg::CompositeMethod::ClipPath);
      if (m_Canvas) m_Canvas->push(std::move(scene));
      else m_Scene->push(std::move(scene));
   }
   else
   {
      if (m_Canvas) m_Canvas->push(std::move(paintWrapper));
      else m_Scene->push(std::move(paintWrapper));
   }
}

void TvgRenderer::drawImageMesh(const RenderImage*, rcp<RenderBuffer> vertices_f32, rcp<RenderBuffer> uvCoords_f32, rcp<RenderBuffer> indices_u16, BlendMode blendMode, float opacity)
{
   // TODO: Implement this!
}

void TvgRenderer::clipPath(RenderPath* path)
{
   //Note: ClipPath transform matrix is calculated by transform matrix in addRenderPath function
   if (!m_BgClipPath)
   {
      m_BgClipPath = static_cast<TvgRenderPath*>(path)->path();
      m_BgClipPath->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   }
   else
   {
      m_ClipPath = static_cast<TvgRenderPath*>(path)->path();
      m_ClipPath->transform({m_Transform[0], m_Transform[2], m_Transform[4], m_Transform[1], m_Transform[3], m_Transform[5], 0, 0, 1});
   }
}

namespace rive
{
   /**
    * @brief Create a buffer of unsigned 16bit data
    * @param data The data to copy to the buffer
    * @return rcp<RenderBuffer> The buffer
    */
   rcp<RenderBuffer> makeBufferU16(Span<const uint16_t> data) { return make_buffer(data); }

   /**
    * @brief Create a buffer of unsigned 32bit data
    * @param data The data to copy to the buffer
    * @return rcp<RenderBuffer> The buffer
    */
   rcp<RenderBuffer> makeBufferU32(Span<const uint32_t> data) { return make_buffer(data); }

   /**
    * @brief Create a buffer of 16bit float data
    * @param data The data to copy to the buffer
    * @return rcp<RenderBuffer> The buffer
    */
   rcp<RenderBuffer> makeBufferF32(Span<const float> data) { return make_buffer(data); }

   /**
    * @brief Create a ThorVG-flavored RenderPath
    * @return RenderPath* The RenderPath
    */
   RenderPath* makeRenderPath()
   {
      return new TvgRenderPath();
   }

   /**
    * @brief Create a ThorVG-flavored RenderPath from the specified information
    * 
    * @param points A span (array) of points
    * @param verbs A span (array) of commands
    * @param fillRule The fill rule to use
    * @return RenderPath* The RenderPath
    */
   RenderPath* makeRenderPath(Span<const rive::Vec2D> points, Span<const uint8_t> verbs, rive::FillRule fillRule)
   {
      // TODO: ThorVG team to check that this method is doing what it is supposed to do!

      TvgRenderPath* renderPath = new TvgRenderPath();
      renderPath->fillRule(fillRule);

      // Build points
      int i;
      Point* coords = new Point[points.size()];
      for (i=0; i<points.size(); i++)
      {
         Point* p = new Point();
         p->x = points[i].x();
         p->y = points[i].y();
         coords[i] = *p;
      }
      renderPath->path()->pathCoords((const Point**)&coords);

      // Build commands
      PathCommand* commands = new PathCommand[verbs.size()];
      for (i=0; i<verbs.size(); i++)
      {
         commands[i] = convertCommand(static_cast<PathVerb>(verbs[i]));
      }
      renderPath->path()->pathCommands((const PathCommand**)&commands);

      return renderPath;
   }

   /**
    * @brief Create a ThorVG-flavored RenderPaint
    * @return RenderPaint* The RenderPaint
    */
   RenderPaint* makeRenderPaint()
   {
      return new TvgRenderPaint();
   }

   /**
    * @brief Create a ThorVG-flavored RenderImage
    * @return RenderImage* The RenderImage
    */
   RenderImage* makeRenderImage()
   {
      return new TvgRenderImage();
   }

   /**
    * @brief Create a linear gradient
    * 
    * @param sx Start X coordinate
    * @param sy Start Y coordinate
    * @param ex End X coordinate
    * @param ey End Y coordinate
    * @param colors Array of colors [count]
    * @param stops Array of stop positions [count]
    * @param count Number of stops
    * @param renderMode Render mode (currently unused?)
    * @param localMatrix The transform
    * @return rcp<RenderShader> Referenced-counted pointer to a RenderShader
    */
   rcp<RenderShader> makeLinearGradient(float sx, float sy, float ex, float ey, const ColorInt colors[], const float stops[], int count, RenderTileMode renderMode, const Mat2D* localMatrix)
   {
      auto gradient = tvg::LinearGradient::gen().release();
      gradient->linear(sx, sy, ex, ey);

      tvg::Fill::ColorStop colorStops[count];
      for (int i = 0; i < count; i++)
      {
         uint8_t r = colors[i] >> 16 & 255;
         uint8_t g = colors[i] >> 8 & 255;
         uint8_t b = colors[i] >> 0 & 255;
         uint8_t a = colors[i] >> 24 & 255;

         colorStops[i] = {stops[i], r, g, b, a};
      }
      gradient->colorStops(colorStops, count);

      return rcp<RenderShader>(new TvgRenderShader(std::unique_ptr<Fill>(gradient)));
   }

   /**
    * @brief Create a radial gradient
    * 
    * @param cx The center X coordinate
    * @param cy The center Y coordinate
    * @param radius The radius
    * @param colors Array of colors [count]
    * @param stops Array of stop positions [count]
    * @param count Number of stops
    * @param renderMode Render mode (currently unused?)
    * @param localMatrix The transform
    * @return rcp<RenderShader> Referenced-counted pointer to a RenderShader
    */
   rcp<RenderShader> makeRadialGradient(float cx, float cy, float radius, const ColorInt colors[], const float stops[], int count, RenderTileMode renderMode, const Mat2D* localMatrix)
   {
      auto gradient = tvg::RadialGradient::gen().release();
      gradient->radial(cx, cy, radius);

      tvg::Fill::ColorStop colorStops[count];
      for (int i = 0; i < count; i++)
      {
         uint8_t r = colors[i] >> 16 & 255;
         uint8_t g = colors[i] >> 8 & 255;
         uint8_t b = colors[i] >> 0 & 255;
         uint8_t a = colors[i] >> 24 & 255;

         colorStops[i] = {stops[i], r, g, b, a};
      }
      gradient->colorStops(colorStops, count);

      return rcp<RenderShader>(new TvgRenderShader(std::unique_ptr<Fill>(gradient)));
   }

   /**
    * @brief Create a sweep gradient
    * TODO: Future work. Rive does not yet support Sweep gradient.
    */
   rcp<RenderShader> makeSweepGradient(float cx, float cy, const ColorInt colors[], const float stops[], int count, const Mat2D* localMatrix)
   {
      return rcp<RenderShader>(nullptr);
   }
}