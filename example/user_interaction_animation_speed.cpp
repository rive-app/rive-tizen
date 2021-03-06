#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "tvg_renderer.hpp"

using namespace std;

#define WIDTH 1000
#define HEIGHT 700
#define LIST_HEIGHT 200

static unique_ptr<tvg::SwCanvas> canvas = nullptr;
static rive::File* currentFile = nullptr;
static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance[2];
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
static double lastTime;
static double animationSpeed = 1.0;

static void deleteWindow(void *data, Evas_Object *obj, void *ev)
{
    elm_exit();
}

static void drawToCanvas(void* data, Eo* obj)
{
    if (canvas->draw() == tvg::Result::Success) canvas->sync();
}

static void loadRiveFile(const char* filename)
{
    lastTime = ecore_time_get();    //Check point

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

    auto animation = artboard->animation(0);
    if (animation) animationInstance[0] = new rive::LinearAnimationInstance(animation);

    animation = artboard->animation(1);
    if (animation) animationInstance[1] = new rive::LinearAnimationInstance(animation);

    delete currentFile;
    currentFile = file;

    delete[] bytes;
}

Eina_Bool animationLoop(void *data)
{
    canvas->clear();

    double currentTime = ecore_time_get();
    float elapsed = currentTime - lastTime;
    lastTime = currentTime;

    // Apply Animation Speed
    elapsed *= animationSpeed;

    if (!artboard) return ECORE_CALLBACK_RENEW;

    for (int i = 0; i < 2; i ++)
    {
       if (animationInstance[i])
       {
         animationInstance[i]->advance(elapsed);
         animationInstance[i]->apply(artboard);
       }
    }

    artboard->advance(elapsed);

    rive::TvgRenderer renderer(canvas.get());
    renderer.save();
    renderer.align(rive::Fit::contain,
                   rive::Alignment::center,
                   rive::AABB(0, 0, WIDTH, HEIGHT),
                   artboard->bounds());
    artboard->draw(&renderer);
    renderer.restore();

    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);

    return ECORE_CALLBACK_RENEW;
}

static void runExample(uint32_t* buffer)
{
    std::string path = RIVE_FILE_DIR;
    path.append("truck063.riv");
    loadRiveFile(path.c_str());

    //Create a Canvas
    canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);
    animator = ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
    for (int i = 0; i < 2; i ++)
    {
       delete animationInstance[i];
       animationInstance[i] = nullptr;
    }
}

void speedChangedCb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   double val = elm_slider_value_get(obj);
   animationSpeed = val;
}

static void setupScreen(uint32_t* buffer)
{
    Eo* win = elm_win_util_standard_add(nullptr, "Rive-Tizen Viewer");
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

    Eo* speedSlider = elm_slider_add(box);
    elm_slider_unit_format_set(speedSlider, "%0.1f x");
    elm_slider_indicator_format_set(speedSlider, "%0.1f");
    elm_slider_span_size_set(speedSlider, 350);
    elm_slider_min_max_set(speedSlider, 0.0, 2.0);
    elm_object_text_set(speedSlider, "Animation Speed : ");
    elm_slider_step_set(speedSlider, 0.1);
    evas_object_size_hint_align_set(speedSlider, EVAS_HINT_FILL, 0.5);
    evas_object_size_hint_weight_set(speedSlider, EVAS_HINT_EXPAND, 0.0);
    elm_slider_value_set(speedSlider, animationSpeed);
    evas_object_smart_callback_add(speedSlider, "changed", speedChangedCb, nullptr);
    evas_object_show(speedSlider);
    elm_box_pack_end(box, speedSlider);

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
