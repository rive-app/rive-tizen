#include "rive_tizen.hpp"

using namespace rive_tizen;

void rive_tizen_print()
{
   // This line to check calling Rive APIs
   rive::AABB aabb(0, 0, 100, 100);
   // This line to check rive_tizen API calls
   std::cout << "hello rive-tizen" << std::endl;
}

bool Controller::loadFile(char *fileName)
{
   //TODO: Implements code for loading rive files
   std::cout << "load File" << std::endl;
   return true;
}

bool Controller::setTargetBuffer(uint32_t* buffer, int width, int height)
{
   //TODO: Implements code for setting target buffer
   return true;
}

bool Controller::applyAnimation(char* animationName, bool on)
{
   //TODO: Implements code for applying rive animation
   return true;
}

double Controller::getDuration()
{
   //TODO: Implements code for Getting rive animation duration
   return true;
}

bool Controller::render(double time)
{
   //TODO: Implements code for Rendering rive animation by time
   return true;
}
