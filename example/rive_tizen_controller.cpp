#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>
#include <iostream>
#include "tvg_renderer.hpp"

using namespace std;

#define WIDTH 1000
#define HEIGHT 700
#define LIST_HEIGHT 200

static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance = nullptr;
static Ecore_Animator* animator = nullptr;
static Eo* view = nullptr;
static vector<std::string> rivefiles;
static double lastTime;
static Eo* statePopup = nullptr;

static rive_tizen::Controller controller;
static void deleteWindow(void* data, Evas_Object* obj, void* ev)
{
	elm_exit();
}

static void drawToCanvas(void* data, Eo* obj)
{
	auto canvas = controller.getCanvas();
	if (canvas->draw() == tvg::Result::Success)
	{
		canvas->sync();
	}
}

static bool isRiveFile(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename) return false;
	return !strcmp(dot + 1, "riv");
}

static void initAnimation(int index)
{
	delete animationInstance;
	animationInstance = nullptr;

	auto animation = artboard->animation(index);
	if (animation) animationInstance = new rive::LinearAnimationInstance(animation, artboard);
}

static void loadRiveFile(const char* filename)
{
	lastTime = ecore_time_get();    //Check point

	if (controller.loadFile(filename) == false) {
		return;
	}

	artboard = controller.getArtboard();
	artboard->advance(0.0f);

	delete animationInstance;
	animationInstance = nullptr;

	auto animation = artboard->firstAnimation();
	if (animation) animationInstance = new rive::LinearAnimationInstance(animation, artboard);
}

static void fileClickedCb(void* data, Evas_Object* obj, void* event_info)
{
	Elm_Object_Item* item = elm_list_selected_item_get(obj);
	int index = 0;
	for (Elm_Object_Item* iter = item; iter != nullptr; iter = elm_list_item_prev(iter))
	{
		index++;
	}
	if (rivefiles.size() > 0) loadRiveFile(rivefiles[index - 1].c_str());
	if (statePopup) elm_ctxpopup_dismiss(statePopup);
}

static std::vector<std::string> riveFiles(const std::string& dirName)
{
	DIR* d;
	struct dirent* dir;
	std::vector<std::string> result;
	d = opendir(dirName.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != nullptr)
		{
			if (isRiveFile(dir->d_name)) result.push_back(dirName + dir->d_name);
		}
		closedir(d);
	}

	std::sort(result.begin(), result.end(), [](auto& a, auto& b) {return a < b; });

	return result;
}

Eina_Bool animationLoop(void* data)
{

	double currentTime = ecore_time_get();
	float elapsed = currentTime - lastTime;
	lastTime = currentTime;

	if (!artboard || !animationInstance) return ECORE_CALLBACK_RENEW;

	animationInstance->advance(elapsed);
	animationInstance->apply();

	controller.render(elapsed);

	evas_object_image_pixels_dirty_set(view, EINA_TRUE);
	evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);

	return ECORE_CALLBACK_RENEW;
}

static void runExample(uint32_t* buffer)
{

	controller.setTargetBuffer(buffer, WIDTH, HEIGHT);
	animator = ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
	delete animationInstance;
	animationInstance = nullptr;
}

static void animPopupItemCb(void* data EINA_UNUSED, Evas_Object* obj, void* event_info)
{
	int animationIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
	initAnimation(animationIndex);
	elm_ctxpopup_dismiss(statePopup);
}

static Elm_Object_Item* animPopupItemNew(Evas_Object* obj, const char* label, int index)
{
	if (!obj) return nullptr;

	return elm_ctxpopup_item_append(obj, label, nullptr, animPopupItemCb, reinterpret_cast<void*>(index));
}

static void animPopupDismissCb(void* data EINA_UNUSED, Evas_Object* obj, void* event_info EINA_UNUSED)
{
	evas_object_del(obj);
	statePopup = nullptr;
}

static void viewClickedCb(void* data, Evas* e EINA_UNUSED, Evas_Object* obj EINA_UNUSED, void* event_info)
{
	if (!artboard) return;
	if (statePopup) evas_object_del(statePopup);

	statePopup = elm_ctxpopup_add(obj);
	evas_object_smart_callback_add(statePopup, "dismissed", animPopupDismissCb, nullptr);

	for (unsigned index = 0; index < artboard->animationCount(); index++)
		animPopupItemNew(statePopup, artboard->animation(index)->name().c_str(), index);

	int x, y;
	evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
	evas_object_move(statePopup, x, y);
	evas_object_show(statePopup);
}

static void setupScreen(uint32_t* buffer)
{
	Eo* win = elm_win_util_standard_add(nullptr, "Rive Viewer");
	evas_object_smart_callback_add(win, "delete,request", deleteWindow, 0);

	Eo* box = elm_box_add(win);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, box);
	evas_object_show(box);

	view = evas_object_image_filled_add(evas_object_evas_get(box));
	evas_object_image_size_set(view, WIDTH, HEIGHT);
	evas_object_image_data_set(view, buffer);
	evas_object_image_pixels_get_callback_set(view, drawToCanvas, nullptr);
	evas_object_image_pixels_dirty_set(view, EINA_TRUE);
	evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);
	evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_min_set(view, WIDTH, HEIGHT);
	evas_object_show(view);

	elm_box_pack_end(box, view);
	evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_UP, viewClickedCb, nullptr);

	Eo* fileList = elm_list_add(box);
	evas_object_size_hint_weight_set(fileList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(fileList, EVAS_HINT_FILL, EVAS_HINT_FILL);

	// Search Rive Files in Resource Dir
	rivefiles = riveFiles(RIVE_FILE_DIR);
	for (size_t i = 0; i < rivefiles.size(); i++)
	{
		const char* ptr = strrchr(rivefiles[i].c_str(), '/');
		elm_list_item_append(fileList, ptr + 1, nullptr, nullptr, fileClickedCb, nullptr);
	}
	elm_list_go(fileList);

	elm_box_pack_end(box, fileList);
	evas_object_show(fileList);

	evas_object_resize(win, WIDTH, HEIGHT + LIST_HEIGHT);
	evas_object_show(win);
}

int main(int argc, char** argv)
{
	static uint32_t buffer[WIDTH * HEIGHT];

	tvg::Initializer::init(tvg::CanvasEngine::Sw, thread::hardware_concurrency());

	elm_init(argc, argv);

	setupScreen(buffer);

	runExample(buffer);

	elm_run();

	cleanExample();

	elm_shutdown();

	tvg::Initializer::term(tvg::CanvasEngine::Sw);

	return 0;
}
