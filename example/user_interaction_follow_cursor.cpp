#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "rive/node.hpp"
#include "rive/animation/linear_animation_instance.hpp"
#include "rive/artboard.hpp"
#include "rive/file.hpp"
#include "tvg_renderer.hpp"

using namespace std;

#define WIDTH 1000
#define HEIGHT 700
#define LIST_HEIGHT 200

static unique_ptr<tvg::SwCanvas> canvas = nullptr;
static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance;
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
static double lastTime;

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
    lastTime = ecore_time_get();

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
    auto file = rive::File::import(reader);
    if (!file)
    {
       delete[] bytes;
       fprintf(stderr, "failed to import %s\n", filename);
       return;
    }

    artboard = file->artboard();
    artboard->advance(0.0f);

    delete animationInstance;
    animationInstance = nullptr;

    auto animation = artboard->firstAnimation();
    if (animation) animationInstance = new rive::LinearAnimationInstance(animation, artboard);

    delete[] bytes;
}

Eina_Bool animationLoop(void *data)
{
    canvas->clear();

    double currentTime = ecore_time_get();
    float elapsed = currentTime - lastTime;
    lastTime = currentTime;

    if (!artboard) return ECORE_CALLBACK_RENEW;

    animationInstance->advance(elapsed);
    animationInstance->apply();

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
    path.append("flame-and-spark.riv");
    loadRiveFile(path.c_str());

    //Create a Canvas
    canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);
    animator = ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
   delete animationInstance;
}

static void mouseMoveCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)event_info;

   int viewx, viewy;
   evas_object_geometry_get(obj, &viewx, &viewy, nullptr, nullptr);

   // Viewx and viewy are the view start position
   int posx = ev->cur.canvas.x - viewx;
   // 250 is the constant for align y center
   int posy = ev->cur.canvas.y - viewy + 250;

   // Get the root instance
   auto root = artboard->find("root");
   auto nodeRoot = root->as<rive::Node>();

   auto spark = artboard->find("spark");
   auto nodeSpark = spark->as<rive::Node>();

   // Set root position
   nodeRoot->x(posx);
   nodeRoot->y(posy);

   // Set spark position, 400 is the constant
   nodeSpark->x(posx - 400);
   nodeSpark->y(posy);
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

    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_MOVE, mouseMoveCb, nullptr);

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
