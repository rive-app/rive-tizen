#include <iostream>

#include "file.hpp"
#include "math/aabb.hpp"
#include "artboard.hpp"
#include "animation/linear_animation_instance.hpp"
#include "tvg_renderer.hpp"

#define RIVE_EXPORT __attribute__ ((visibility ("default")))

RIVE_EXPORT void rive_tizen_print();

namespace rive_tizen
{
   class Controller
   {
      rive::Artboard* m_Artboard;
   public:
      Controller(){}
      bool loadFile(char *fileName);
      bool setTargetBuffer(uint32_t* buffer, int width, int height);
      bool applyAnimation(char* animationName, bool on);
      double getDuration();
      bool render(double time);
   };
}
