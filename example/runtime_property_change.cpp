#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "shapes/paint/fill.hpp"
#include "shapes/paint/stroke.hpp"
#include "shapes/paint/color.hpp"
#include "shapes/paint/solid_color.hpp"
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
static rive::LinearAnimationInstance* animationInstance = nullptr;
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
static vector<std::string> rivefiles;
static double lastTime;
static Eo* statePopup = nullptr;

std::string currentColorInstance;
Eo *entryR, *entryG, *entryB, *entryA;

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
    animationInstance = nullptr;

    auto animation = artboard->firstAnimation();
    if (animation) animationInstance = new rive::LinearAnimationInstance(animation);

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
    path.append("runtime_color_change.riv");
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

static void animPopupItemCb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info)
{
    int animationIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
    initAnimation(animationIndex);
    elm_ctxpopup_dismiss(statePopup);
}

static Elm_Object_Item* animPopupItemNew(Evas_Object *obj, const char *label, int index)
{
    Elm_Object_Item *it = nullptr;

    if (!obj) return nullptr;

    return elm_ctxpopup_item_append(obj, label, nullptr, animPopupItemCb, (void*)index);
}

static void animPopupDismissCb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    evas_object_del(obj);
    statePopup = nullptr;
}

static void viewClickedCb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
    if (!artboard) return;
    if (statePopup) evas_object_del(statePopup);

    statePopup = elm_ctxpopup_add(obj);
    evas_object_smart_callback_add(statePopup, "dismissed", animPopupDismissCb, nullptr);

    for (int index = 0; index < artboard->animationCount(); index++)
      animPopupItemNew(statePopup, artboard->animation(index)->name().c_str(), index);

    int x, y;
    evas_pointer_canvas_xy_get(evas_object_evas_get(obj), &x, &y);
    evas_object_move(statePopup, x, y);
    evas_object_show(statePopup);
}

static void selectedCb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   const char *text = elm_object_item_text_get((Elm_Object_Item*)event_info);
   currentColorInstance = text;
}

static void applyCb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   const char *r = elm_object_text_get(entryR);
   const char *g = elm_object_text_get(entryG);
   const char *b = elm_object_text_get(entryB);
   const char *a = elm_object_text_get(entryA);

   printf("current vector instance: %s r:%d g:%d b:%d a:%d\n", currentColorInstance.c_str(), atoi(r), atoi(g), atoi(b), atoi(a));

   auto colorInstance = artboard->find<rive::Fill>(currentColorInstance.c_str());
   if (colorInstance)
     colorInstance->paint()->as<rive::SolidColor>()->colorValue(rive::colorARGB(atoi(a), atoi(r), atoi(g), atoi(b)));
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

    Eo* hoversel = elm_hoversel_add(win);
    elm_hoversel_auto_update_set(hoversel, EINA_TRUE);
    elm_hoversel_hover_parent_set(hoversel, win);
    evas_object_smart_callback_add(hoversel, "selected", selectedCb, NULL);
    evas_object_size_hint_weight_set(hoversel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(hoversel, EVAS_HINT_FILL, 0.0);
    elm_object_text_set(hoversel, "Vector Instances");
    elm_hoversel_item_add(hoversel, "body_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "straw_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "eye_left_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "eye_right_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "mouse_color", NULL, ELM_ICON_NONE, NULL, NULL);
    evas_object_show(hoversel);

    elm_box_pack_end(box, hoversel);

    Eo* colorBox = elm_box_add(win);
    elm_box_horizontal_set(colorBox, true);
    evas_object_size_hint_weight_set(colorBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(colorBox, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(box, colorBox);

    Eo* colorRText = elm_label_add(colorBox);
    elm_object_text_set(colorRText, "R : ");
    evas_object_size_hint_weight_set(colorRText, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(colorRText);

    entryR = elm_entry_add(colorBox);
    elm_entry_scrollable_set(entryR, EINA_TRUE);
    elm_entry_single_line_set(entryR, EINA_TRUE);
    elm_scroller_policy_set(entryR, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
    evas_object_size_hint_weight_set(entryR, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entryR, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entryR);

    elm_box_pack_end(colorBox, colorRText);
    elm_box_pack_end(colorBox, entryR);

    Eo* colorGText = elm_label_add(colorBox);
    elm_object_text_set(colorGText, "G : ");
    evas_object_size_hint_weight_set(colorGText, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(colorGText);

    entryG = elm_entry_add(colorBox);
    elm_entry_single_line_set(entryG, EINA_TRUE);
    elm_entry_scrollable_set(entryG, EINA_TRUE);
    elm_entry_single_line_set(entryG, EINA_TRUE);
    evas_object_size_hint_weight_set(entryG, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entryG, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entryG);

    elm_box_pack_end(colorBox, colorGText);
    elm_box_pack_end(colorBox, entryG);

    Eo* colorBText = elm_label_add(colorBox);
    elm_object_text_set(colorBText, "B : ");
    evas_object_size_hint_weight_set(colorBText, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(colorBText);

    entryB = elm_entry_add(colorBox);
    elm_entry_single_line_set(entryB, EINA_TRUE);
    elm_entry_scrollable_set(entryB, EINA_TRUE);
    elm_entry_single_line_set(entryB, EINA_TRUE);
    evas_object_size_hint_weight_set(entryB, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entryB, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entryB);

    elm_box_pack_end(colorBox, colorBText);
    elm_box_pack_end(colorBox, entryB);

    Eo* colorAText = elm_label_add(colorBox);
    elm_object_text_set(colorAText, "A : ");
    evas_object_size_hint_weight_set(colorAText, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(colorAText);

    entryA = elm_entry_add(colorBox);
    elm_entry_single_line_set(entryA, EINA_TRUE);
    elm_entry_scrollable_set(entryA, EINA_TRUE);
    elm_entry_single_line_set(entryA, EINA_TRUE);
    evas_object_size_hint_weight_set(entryA, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entryA, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(entryA);

    elm_box_pack_end(colorBox, colorAText);
    elm_box_pack_end(colorBox, entryA);

    Eo* applyButton = elm_button_add(colorBox);
    elm_object_text_set(applyButton, "Apply");
    evas_object_smart_callback_add(applyButton, "clicked", applyCb, nullptr);
    evas_object_show(applyButton);
    elm_box_pack_end(colorBox, applyButton); 

    evas_object_show(colorBox);

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
