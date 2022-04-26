#include <iostream>

#include "rive/file.hpp"
#include "rive/math/aabb.hpp"
#include "rive/artboard.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "tvg_renderer.hpp"

#ifdef _MSC_VER
#define RIVE_EXPORT __declspec( dllexport )
#else
#define RIVE_EXPORT __attribute__ ((visibility ("default")))
#endif

RIVE_EXPORT void rive_tizen_print();

namespace rive_tizen
{
	class Controller
	{
	public:
		Controller();
		bool loadFile(const char* fileName);
		bool setTargetBuffer(uint32_t* buffer, int width, int height);
		bool applyAnimation(char* animationName, bool on);
		double getDuration();
		tvg::SwCanvas* getCanvas();
		bool render(double time);

		rive::Artboard* getArtboard();

	private:
		rive::File* m_File;
		std::unique_ptr<tvg::SwCanvas> m_Canvas;
		bool m_Is_Fileloaded;

		int m_Width;
		int m_Height;
	};
}
