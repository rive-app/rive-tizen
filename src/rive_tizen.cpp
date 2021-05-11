#include "rive_tizen.hpp"
#include "math/aabb.hpp"

void rive_tizen_print()
{
   // This line to check calling Rive APIs
   rive::AABB aabb(0, 0, 100, 100);
   // This line to check rive_tizen API calls
   std::cout << "hello rive-tizen" << std::endl;
}


namespace rive
{
   namespace tizen
   {
      class Controller
      {
         rive::Arboard* m_Artboard;
      public:
         Controller(){}
         bool loadFile(char *fileName)
         {
            //TODO: Implements code for loading rive files
            FILE* fp = fopen(fileName, "r");
            fseek(fp, 0, SEEK_END);
            size_t length = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            uint8_t* bytes = new uint8_t[length];
            auto reader = rive::BinaryReader(bytes, length);
            rive::File* file = nullptr;
            auto result = rive::File::import(reader, &file);

            m_Artboard = file->artboard();
         }
         bool setTargetBuffer(uint32_t* buffer, int width, int height)
         {
            //TODO: Implements code for setting target buffer
         }

         bool applyAnimation(char* animationName, bool on)
         {
            //TODO: Implements code for applying rive animation
         }

         double getDuration()
         {
            //TODO: Get rive animation duration
         }

         bool render(double time)
         {
            //TODO: Render rive animation by time
         }
      }
   }
}
