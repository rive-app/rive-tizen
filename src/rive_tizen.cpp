#include "rive_tizen.hpp"
#include "math/aabb.hpp"

void rive_tizen_print()
{
   // This line to check calling Rive APIs
   rive::AABB aabb(0, 0, 100, 100);
   // This line to check rive_tizen API calls
   std::cout << "hello rive-tizen" << std::endl;
}
