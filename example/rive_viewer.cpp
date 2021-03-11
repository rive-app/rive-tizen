#include <thread>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "thorvg_renderer.hpp"

using namespace std;

#define WIDTH 700
#define HEIGHT 700

static unique_ptr<tvg::SwCanvas> canvas;
static tvg::Canvas *renderCanvas;
static rive::File* file = nullptr;
static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance = nullptr;
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
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

    // Load Rive File
    const char* filename = "../../example/shapes.riv";
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

    auto animation = artboard->firstAnimation<rive::LinearAnimation>();
    if (animation != nullptr)
    {
       animationInstance = new rive::LinearAnimationInstance(animation);
    }
    else
    {
       animationInstance = nullptr;
    }

    lastTime = ecore_time_get();
    ecore_animator_frametime_set(1. / 60);
    animator = ecore_animator_add(animationLoop, nullptr);

    delete[] bytes;
}


static void cleanExample()
{
    delete file;
    delete animationInstance;
}


static void setupScreen(uint32_t* buffer)
{
    Eo* win = elm_win_util_standard_add(NULL, "Rive Viewer");
    evas_object_smart_callback_add(win, "delete,request", deleteWindow, 0);

    view = evas_object_image_filled_add(evas_object_evas_get(win));
    evas_object_image_size_set(view, WIDTH, HEIGHT);
    evas_object_image_data_set(view, buffer);
    evas_object_image_pixels_get_callback_set(view, drawToCanvas, nullptr);
    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);
    evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(view);

    elm_win_resize_object_add(win, view);
    evas_object_resize(win, WIDTH, HEIGHT);
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
