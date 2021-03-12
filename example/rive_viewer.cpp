#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "thorvg_renderer.hpp"

using namespace std;

#define WIDTH 1000
#define HEIGHT 700
#define LIST_HEIGHT 200

static unique_ptr<tvg::SwCanvas> canvas;
static tvg::Canvas *renderCanvas;
static rive::File* currentFile = nullptr;
static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance = nullptr;
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
static vector<std::string> rivefiles;
static double lastTime;

static void deleteWindow(void *data, Evas_Object *obj, void *ev)
{
    elm_exit();
}

static void drawToCanvas(void* data, Eo* obj)
{
    if (canvas->draw() == tvg::Result::Success)
    {
        canvas->sync();
    }
}

static bool isRiveFile(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return false;
    return !strcmp(dot + 1, "riv");
}

static void loadRiveFile(const char* filename)
{
    // Clear Canvas Buffer
    renderCanvas->clear();

    // Load Rive File
    FILE* fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t* bytes = new uint8_t[length];
    if (fread(bytes, 1, length, fp) != length)
    {
       delete[] bytes;
       fprintf(stderr, "failed to read all of %s\n", filename);
       return;
    }

    auto reader = rive::BinaryReader(bytes, length);
    rive::File* file = nullptr;
    auto result = rive::File::import(reader, &file);
    if (result != rive::ImportResult::success)
    {
       delete[] bytes;
       fprintf(stderr, "failed to import %s\n", filename);
       return;
    }

    artboard = file->artboard();
    artboard->advance(0.0f);

    delete animationInstance;
    delete currentFile;

    auto animation = artboard->firstAnimation<rive::LinearAnimation>();
    if (animation != nullptr)
    {
       animationInstance = new rive::LinearAnimationInstance(animation);
    }
    else
    {
       animationInstance = nullptr;
    }

    currentFile = file;
    delete[] bytes;
}

static void fileClickedCb (void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *item = elm_list_selected_item_get(obj);
    int index = 0;
    for (Elm_Object_Item *iter = item; iter != NULL; iter = elm_list_item_prev(iter))
       index++;
    if (rivefiles.size() > 0)
      loadRiveFile(rivefiles[index-1].c_str());
}

static std::vector<std::string> riveFiles(const std::string &dirName)
{
    DIR *d;
    struct dirent *dir;
    std::vector<std::string> result;
    d = opendir(dirName.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (isRiveFile(dir->d_name))
          result.push_back(dirName + dir->d_name);
      }
      closedir(d);
    }

    std::sort(result.begin(), result.end(), [](auto & a, auto &b){return a < b;});

    return result;
}

Eina_Bool animationLoop(void *data)
{
    double currentTime = ecore_time_get();
    float elapsed = currentTime - lastTime;
    static float animationTime = 0;
    lastTime = currentTime;

    if (artboard != nullptr)
    {
       if (animationInstance != nullptr)
       {
          animationInstance->advance(elapsed);
          animationInstance->apply(artboard);
       }
       artboard->advance(elapsed);

       rive::TvgRenderer renderer(renderCanvas);
       renderer.save();
       artboard->draw(&renderer);
       renderer.restore();
    }

    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);

    return ECORE_CALLBACK_RENEW;
}

static void runExample(uint32_t* buffer)
{
    //Create a Canvas
    canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);
    renderCanvas = canvas.get();

    lastTime = ecore_time_get();
    ecore_animator_frametime_set(1. / 60);
    animator = ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
    delete animationInstance;
}

static void setupScreen(uint32_t* buffer)
{
    Eo* win = elm_win_util_standard_add(NULL, "Rive Viewer");
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

    Eo *fileList = elm_list_add(box);
    evas_object_size_hint_weight_set(fileList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fileList, EVAS_HINT_FILL, EVAS_HINT_FILL);

    // Search Rive Files in Resource Dir
    rivefiles = riveFiles(RIVE_FILE_DIR);
    for (int i = 0; i < rivefiles.size(); i++)
    {
       const char *ptr = strrchr(rivefiles[i].c_str(), '/');
       Elm_Object_Item *item = elm_list_item_append(fileList, ptr + 1, NULL, NULL, fileClickedCb, NULL);
    }
    elm_list_go(fileList);

    elm_box_pack_end(box, fileList);
    evas_object_show(fileList);

    evas_object_resize(win, WIDTH, HEIGHT + LIST_HEIGHT);
    evas_object_show(win);
}

int main(int argc, char **argv)
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
