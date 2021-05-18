#include <thread>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "node.hpp"
#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "tvg_renderer.hpp"

#define WIDTH 1000
#define HEIGHT 1000

static unique_ptr<tvg::SwCanvas> canvas = nullptr;
static rive::Artboard* artboard = nullptr;
static rive::LinearAnimationInstance* animationInstance;
static Eo* view = nullptr;
static double lastTime;
int isOpen = true;

static void deleteWindow(void *data, Evas_Object *obj, void *ev)
{
    elm_exit();
}

static void drawToCanvas(void* data, Eo* obj)
{
    if (canvas->draw() == tvg::Result::Success) canvas->sync();
}

static void initAnimation(int index)
{
    delete animationInstance;
    animationInstance = nullptr;

    auto animation = artboard->animation(index);
    if (animation) animationInstance = new rive::LinearAnimationInstance(animation);
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
       fclose(fp);
       return;
    }

    auto reader = rive::BinaryReader(bytes, length);
    rive::File* file = nullptr;
    auto result = rive::File::import(reader, &file);
    if (result != rive::ImportResult::success)
    {
       delete[] bytes;
       fprintf(stderr, "failed to import %s\n", filename);
       fclose(fp);
       return;
    }

    artboard = file->artboard();
    artboard->advance(0.0f);

    auto animation = artboard->firstAnimation();
    if (animation) animationInstance = new rive::LinearAnimationInstance(animation);

    delete[] bytes;
    fclose(fp);
}

Eina_Bool animationLoop(void *data)
{
    canvas->clear();

    double currentTime = ecore_time_get();
    float elapsed = currentTime - lastTime;
    lastTime = currentTime;

    if (!artboard || !animationInstance) return ECORE_CALLBACK_RENEW;

    artboard->updateComponents();

    animationInstance->advance(elapsed);
    animationInstance->apply(artboard);

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
    path.append("barrier.riv");
    loadRiveFile(path.c_str());
    printf("Please touch the star to open and close the barrier!\n");

    //Create a Canvas
    canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);
    ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
    delete animationInstance;
    animationInstance = nullptr;
}

static void mouseUpCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;

   int up_x = ev->canvas.x;
   int up_y = ev->canvas.y;

   auto root = artboard->find("Star");
   auto nodeRoot = root->as<rive::Node>();

   // Note: The worldTransform values are based on the artboard size.
   // If the renderer alignment transform is applied
   // since the view raito is different with the artboard ratio,
   // The artboard scale and start position should be considered
   // to match the touch position.
   rive::Mat2D mat = nodeRoot->worldTransform();

   float nodePosX = mat[4];
   float nodePosY = mat[5];

   float distance = sqrt(pow(up_x - nodePosX, 2) + pow(up_y - nodePosY, 2));
   // 10 is the constant value for touching area
   if (distance < 10)
   {
      if (isOpen)
      {
        initAnimation(2);
        isOpen = false;
        printf("close\n");
      }
      else
      {
        initAnimation(0);
        isOpen = true;
        printf("open\n");
      }
   }
}

static void setupScreen(uint32_t* buffer)
{
    Eo* win = elm_win_util_standard_add(nullptr, "Rive-Tizen Viewer");
    evas_object_smart_callback_add(win, "delete,request", deleteWindow, 0);

    view = evas_object_image_filled_add(evas_object_evas_get(win));
    evas_object_image_size_set(view, WIDTH, HEIGHT);
    evas_object_image_data_set(view, buffer);
    evas_object_image_pixels_get_callback_set(view, drawToCanvas, nullptr);
    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);
    evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_min_set(view, WIDTH, HEIGHT);
    evas_object_show(view);
    elm_win_resize_object_add(win, view);

    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_UP, mouseUpCb, nullptr);

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
