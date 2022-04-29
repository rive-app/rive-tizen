#ifndef _RIVE_THORVG_RENDERER_HPP_
#define _RIVE_THORVG_RENDERER_HPP_

#include <thorvg.h>
#include <vector>
#include <stack>
#include "rive/renderer.hpp"

using namespace tvg;

namespace rive
{

   /**
    * @brief Shader
    * A shader represents a gradient fill or an image fill.
    */
   class TvgRenderShader : public RenderShader {
   private:
      std::unique_ptr<Fill> m_Fill = nullptr;
      Picture* m_Picture = nullptr;
      Mat2D m_Transform;
   public:
      /**
       * @brief Construct a new Render Shader with a gradient fill
       * @param fill The gradient fill
       */
      TvgRenderShader(std::unique_ptr<Fill> fill) : m_Fill(std::move(fill)){}

      /**
       * @brief Construct a new Tvg Render Shader with an image fill
       * @param picture The image fill
       */
      TvgRenderShader(Picture* picture, const Mat2D* transform = nullptr) : m_Picture(picture){
         if (transform) {
            m_Transform[0] = (*transform)[0];
            m_Transform[1] = (*transform)[1];
            m_Transform[2] = (*transform)[2];
            m_Transform[3] = (*transform)[3];
            m_Transform[4] = (*transform)[4];
            m_Transform[5] = (*transform)[5];
         }
      }

      /**
       * @brief Check if the shader holds a gradient fill
       * 
       * @return true If the shader holds a gradient fill
       * @return false If the shader does not hold a gradient fill
       */
      bool hasFill(){ return (bool)m_Fill; }

      /**
       * @brief Get the gradient fill
       * The fill pointer is now owned by the caller
       * @return unique_ptr<tvg::Fill> The gradient fill (or nullptr)
       */
      Fill* fill() { return m_Fill.get(); }

      /**
       * @brief Check if the shader holds a picture
       * 
       * @return true If the shader holds a picture
       * @return false If the shader does not hold a picture
       */
      bool hasPicture(){ return m_Picture != nullptr; }

      /**
       * @brief Get the image fill
       * @return Picture* The image fill (or nullptr)
       */
      Picture* picture() { return m_Picture; }
   };

   /**
    * @brief A struct that describes a paint operation
    */
   class TvgPaint
   {
   public:
      uint8_t color[4];
      float thickness = 1.0f;
      std::unique_ptr<TvgRenderShader> shader = nullptr;
      tvg::StrokeJoin join = tvg::StrokeJoin::Bevel;
      tvg::StrokeCap  cap = tvg::StrokeCap::Butt;
      RenderPaintStyle style = RenderPaintStyle::fill;

      /**
       * @brief Check if this paint describes a gradient fill
       */
      bool isFill(){ return shader && shader->hasFill(); }

      /**
       * @brief Check if this paint describes an image fill
       */
      bool isPicture(){ return shader && shader->hasPicture(); }
   };

   /**
    * @brief ThorVG implementation of RenderPath
    */
   class TvgRenderPath : public RenderPath
   {
   private:
      std::unique_ptr<Shape> m_Path;
   public:
      /**
       * @brief Construct a new Render Path object
       */
      TvgRenderPath() : m_Path(tvg::Shape::gen()) {}

      /**
       * @brief Get the path
       */
      Shape* path() const { return m_Path.get(); }

      /**
       * @brief Reset the path
       */
      void reset() override;

      /**
       * @brief Append a path
       * 
       * @param path The path to append
       * @param transform The transform
       */
      void addRenderPath(RenderPath* path, const Mat2D& transform) override;

      /**
       * @brief Set the fill rule
       */
      void fillRule(FillRule value) override;

      /**
       * @brief Move to a point without drawing a line
       * @param x The X coordinate
       * @param y The Y coordinate
       */
      void moveTo(float x, float y) override;

      /**
       * @brief Draw line from current position to a point
       * @param x The X coordinate
       * @param y The Y coordinate
       */
      void lineTo(float x, float y) override;

      /**
       * @brief Draw cubic curve from current position to a point
       * @param ox First control point X coordinate
       * @param oy First control point Y coordinate
       * @param ix Second control point X coordinate
       * @param iy Second control point Y coordinate
       * @param x The X coordinate
       * @param y The Y coordinate
       */
      void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;

      /**
       * @brief Close the current shape
       */
      virtual void close() override;
   };

   /**
    * @brief ThorVG implementation of RenderPaint
    */
   class TvgRenderPaint : public RenderPaint
   {
   private:
      TvgPaint m_Paint;
   public:
      /**
       * @brief Get the paint
       */
      TvgPaint* paint() { return &m_Paint; }

      /**
       * @brief Set the style
       * @param style The style (stroke, fill)
       */
      void style(RenderPaintStyle style) override;

      /**
       * @brief Set the color ARGB
       * @param value The color in ARGB format
       */
      void color(unsigned int value) override;

      /**
       * @brief Set the thickness
       */
      void thickness(float value) override;

      /**
       * @brief Set the join style
       */
      void join(StrokeJoin value) override;

      /**
       * @brief Set the cap style
       */
      void cap(StrokeCap value) override;

      /**
       * @brief Set the blend mode
       */
      void blendMode(BlendMode value) override {}

      /**
       * @brief Set the shader
       * @param shader The shader
       */
      void shader(rcp<RenderShader> shader) override;
   };

   /**
    * @brief ThorVG implementation of RenderImage
    */
   class TvgRenderImage : public RenderImage {
   private:
      std::unique_ptr<Picture> m_Image = nullptr;
   public:
      /**
       * @brief Image width
       * Set during decode. Should be read-only
       */
      float width = 0.0;

      /**
       * @brief Image height
       * Set during decode. Should be read-only
       */
      float height = 0.0;

      /**
       * @brief Get the image as a Picture
       */
      Picture* image() { return m_Image.get(); };

      /**
       * @brief Decode an image
       * @return true Succeeded
       * @return false Failed
       */
      bool decode(Span<const uint8_t>) override;

      /**
       * @brief Create an image fill shader from this RenderImage
       * @param tx The tiling mode in the X direction
       * @param ty The tiling mode in the Y direction
       * @param localMatrix The transform matrix
       * @return rcp<RenderShader> An image fill shader
       */
      rcp<RenderShader> makeShader(RenderTileMode tx, RenderTileMode ty, const Mat2D* localMatrix) const override;
   };

   /**
    * @brief State for save and restore calls
    */
   struct TvgRendererState
   {
      Shape* clipPath;
      Mat2D transform;
   };

   /**
    * @brief Renders rive files
    * 
    */
   class TvgRenderer : public Renderer
   {
   protected:
      Canvas* m_Canvas = nullptr;
      Scene* m_Scene = nullptr;
      Shape* m_ClipPath = nullptr;
      Mat2D m_Transform;
      Shape* m_BgClipPath = nullptr;
      std::stack<TvgRendererState> m_SavedTransforms;
   public:
      /**
       * @brief Construct a new Renderer that draws direct to canvas
       * @param canvas The canvas to render to
       */
      TvgRenderer(Canvas* canvas) : m_Canvas(canvas) {}

      /**
       * @brief Construct a new Renderer that draws to a scene
       * @param canvas The canvas to render to
       */
      TvgRenderer(Scene* scene) : m_Scene(scene) {}

      /**
       * @brief Save the current transform
       * @see restore
       */
      void save() override;

      /**
       * @brief Restore the last saved transform
       * @see save
       */
      void restore() override;

      /**
       * @brief Apply a transform
       * Multiplies the transform
       * @param transform The matrix
       */
      void transform(const Mat2D& transform) override;

      /**
       * @brief Set the current clipping path
       * @param path The clip path
       */
      void clipPath(RenderPath* path) override;

      /**
       * @brief Draw a path to the tvg canvas
       * 
       * @param path The path to draw 
       * @param paint The paint to use while drawing
       */
      void drawPath(RenderPath* path, RenderPaint* paint) override;

      /**
       * @brief Draw an image to the tvg canvas
       * @param image The image to draw 
       * @param blendMode The blending mode - currently unsupported by ThorVG? 
       * @param opacity The opacity from 0.0 to 1.0
       */
      void drawImage(const RenderImage* image, BlendMode blendMode, float opacity) override;

      /**
       * @brief Draw a mesh of images to the tvg canvas
       * @param image The image to draw 
       * @param vertices_f32 
       * @param uvCoords_f32 
       * @param indices_u16 
       * @param blendMode The blending mode - currently unsupported by ThorVG? 
       * @param opacity The opacity from 0.0 to 1.0
       */
      void drawImageMesh(const RenderImage* image, rcp<RenderBuffer> vertices_f32, rcp<RenderBuffer> uvCoords_f32, rcp<RenderBuffer> indices_u16, BlendMode blendMode, float opacity) override;
   };

} // namespace

#endif
