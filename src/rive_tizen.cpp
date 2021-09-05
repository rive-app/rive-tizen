#include <fstream>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <iterator>

#include "rive_tizen.hpp"
using namespace rive_tizen;

void rive_tizen_print()
{
	// This line to check calling Rive APIs
	rive::AABB aabb(0, 0, 100, 100);
	// This line to check rive_tizen API calls
	std::cout << "hello rive-tizen" << std::endl;
}


Controller::Controller() : m_Is_Fileloaded(false), m_File(nullptr) {
}

bool Controller::loadFile(const char* fileName)
{
	if (m_File != NULL)
	{
		delete m_File;
		m_File = nullptr;
	}
	m_Is_Fileloaded = false;
	m_Canvas->clear();

	std::ifstream fp(fileName, std::ios::binary);

	if (fp.is_open() == false)
	{
		return false;
	}

	std::vector<uint8_t> binaryData(std::istreambuf_iterator<char>(fp), {});

	std::size_t length = binaryData.size();

	auto reader = rive::BinaryReader(binaryData.data(), length);
	auto result = rive::File::import(reader, &m_File);

	if (result != rive::ImportResult::success)
	{
		fprintf(stderr, "failed to import %s\n", fileName);
		return false;
	}

	m_Is_Fileloaded = true;
	return true;
}

bool Controller::setTargetBuffer(uint32_t* buffer, int width, int height)
{
	m_Width = width;
	m_Height = height;
	m_Canvas = tvg::SwCanvas::gen();

	m_Canvas->target(buffer, width, width, height, tvg::SwCanvas::ARGB8888);
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
	auto artboard = getArtboard();
	if (artboard == nullptr) {
		return NAN;
	}
	auto animation = artboard->firstAnimation();
	int start = animation->enableWorkArea() ? animation->workStart() : 0;
	int end =
		animation->enableWorkArea() ? animation->workEnd() : animation->duration();
	int range = end - start;

	return range / animation->fps();
}

tvg::SwCanvas* rive_tizen::Controller::getCanvas() {
	return m_Canvas.get();
}

bool Controller::render(double elapsed)
{
	m_Canvas->clear();

	auto artboard = this->getArtboard();
	if (artboard == nullptr)
	{
		return false;
	}
	artboard->advance(elapsed);

	rive::TvgRenderer renderer(this->getCanvas());
	renderer.save();
	renderer.align(rive::Fit::contain,
		rive::Alignment::center,
		rive::AABB(0, 0, m_Width, m_Height),
		artboard->bounds());
	artboard->draw(&renderer);
	renderer.restore();

	return true;
}
rive::Artboard* Controller::getArtboard() {
	if (m_File == nullptr)
	{
		return nullptr;
	}
	return m_File->artboard();
}