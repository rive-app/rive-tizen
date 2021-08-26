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
		unique_ptr<tvg::SwCanvas> m_Canvas;
		bool m_Is_Fileloaded;

		int m_Width;
		int m_Height;
	};
}
