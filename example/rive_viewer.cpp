#include <thread>
#include <Evas_GL.h>
#include <Elementary.h>
#include <rive_tizen.hpp>
#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "thorvg_renderer.hpp"

using namespace std;


#define WIDTH 700
#define HEIGHT 700

static uint32_t buffer[WIDTH * HEIGHT];
static unique_ptr<tvg::SwCanvas> swCanvas;
Eo *gView;
rive::File* currentFile = nullptr;
rive::Artboard* artboard = nullptr;
rive::LinearAnimationInstance* animationInstance = nullptr;

void win_del(void *data, Evas_Object *o, void *ev)
{
   elm_exit();
}

void drawSwView(void* data, Eo* obj)
{
    if (swCanvas && swCanvas->draw() == tvg::Result::Success) {
        swCanvas->sync();
    }
}

void tvgSwTest(uint32_t* buffer)
{
    //Create a Canvas
    swCanvas = tvg::SwCanvas::gen();
    swCanvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);

    // Load Rive File
    char *filename = "../../example/shapes.riv";
    FILE* fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);
    auto length = ftell(fp);
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

    if (animationInstance != nullptr)
    {
       animationInstance->advance(0);
       animationInstance->apply(artboard);
    }
    artboard->advance(0);

    rive::ThorvgRenderer renderer(swCanvas.get());
    renderer.save();
    artboard->draw(&renderer);
    renderer.restore();
}

static Eo* createSwView()
{
    Eo* win = elm_win_util_standard_add(NULL, "Rive Viewer");
    evas_object_smart_callback_add(win, "delete,request", win_del, 0);

    Eo* view = evas_object_image_filled_add(evas_object_evas_get(win));
    evas_object_image_size_set(view, WIDTH, HEIGHT);
    evas_object_image_data_set(view, buffer);
    evas_object_image_pixels_get_callback_set(view, drawSwView, nullptr);
    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);
    evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(view);

    elm_win_resize_object_add(win, view);
    evas_object_resize(win, WIDTH, HEIGHT);
    evas_object_show(win);

    tvgSwTest(buffer);

    return view;
}

int main(int argc, char **argv)
{
    rive_tizen_print();
    rive::ThorvgRenderPath *path = new rive::ThorvgRenderPath();

    tvg::CanvasEngine tvgEngine = tvg::CanvasEngine::Sw;

    auto threads = std::thread::hardware_concurrency();

    if (tvg::Initializer::init(tvgEngine, threads) == tvg::Result::Success)
    {
        elm_init(argc, argv);

        gView = createSwView();

        elm_run();
        elm_shutdown();

        tvg::Initializer::term(tvgEngine);

    }
    else
    {
        cout << "engine is not supported" << endl;
    }

    return 0;
}
