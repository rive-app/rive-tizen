#include <thread>
#include <dirent.h>
#include <algorithm>
#include <Elementary.h>
#include <rive_tizen.hpp>

#include "node.hpp"
#include "shapes/paint/fill.hpp"
#include "shapes/paint/stroke.hpp"
#include "shapes/paint/color.hpp"
#include "shapes/paint/solid_color.hpp"
#include "animation/linear_animation_instance.hpp"
#include "artboard.hpp"
#include "file.hpp"
#include "tvg_renderer.hpp"

using namespace std;

#define WIDTH 720
#define HEIGHT 720
#define INFOWIDTH 560
static Eo* win = nullptr;
static unique_ptr<tvg::SwCanvas> canvas = nullptr;
static rive::File* currentFile = nullptr;
static rive::Artboard* artboard = nullptr;
bool   enableAnimation[30];
static rive::LinearAnimationInstance* animationInstance[30];
static Ecore_Animator *animator = nullptr;
static Eo* view = nullptr;
static vector<std::string> rivefiles;
static double lastTime;
static Eo* statePopup = nullptr;

std::vector<Eo*> infoWidgets;


static void deleteWindow(void *data, Evas_Object *obj, void *ev)
{
    elm_exit();
}

static void drawToCanvas(void* data, Eo* obj)
{
    if (canvas->draw() == tvg::Result::Success) canvas->sync();
}

static bool isRiveFile(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return false;
    return !strcmp(dot + 1, "riv");
}

static void initAnimation(int index)
{
/*
    delete animationInstance;
    animationInstance = nullptr;

    auto animation = artboard->animation(index);
    if (animation) animationInstance = new rive::LinearAnimationInstance(animation);
*/
}

static void loadRiveFile(const char* filename)
{
    lastTime = ecore_time_get();    //Check point
    canvas->clear();            //Clear Canvas Buffer

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

    for (int i = 0; i<artboard->animationCount(); i++)
    {
       delete animationInstance[i];
       animationInstance[i] = nullptr;
    }

    for (int i = 0; i<artboard->animationCount(); i++)
    {
       auto animation = artboard->animation(i);
       if (animation) animationInstance[i] = new rive::LinearAnimationInstance(animation);
       printf("KTH duration:%f\n", animation->durationSeconds());
    }

    delete currentFile;
    currentFile = file;

    delete[] bytes;
}

//State Based Animation
static void demo1SelectedCb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
    const char *text = elm_object_item_text_get((Elm_Object_Item*)event_info);
    int animationIndex = 3;
    if (!strcmp(text, "Idle"))
      animationIndex = 3;
    else if (!strcmp(text, "Happy"))
      animationIndex = 1;
    else if (!strcmp(text, "Speak"))
      animationIndex = 2;
    else if (!strcmp(text, "Wrong"))
      animationIndex = 0;

    for (int i=0; i < artboard->animationCount(); i++)
    {
       enableAnimation[i] = false;
       delete animationInstance[i];
       animationInstance[i] = nullptr;
       auto animation = artboard->animation(i);
       if (animation) animationInstance[i] = new rive::LinearAnimationInstance(animation);
    }

    enableAnimation[animationIndex] = true;
}

static void demo1()
{
    Eo* hoversel = elm_hoversel_add(win);
    elm_hoversel_auto_update_set(hoversel, EINA_TRUE);
    elm_hoversel_hover_parent_set(hoversel, win);
    evas_object_smart_callback_add(hoversel, "selected", demo1SelectedCb, NULL);
    evas_object_size_hint_weight_set(hoversel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(hoversel, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(hoversel, "Vector Instances");
    elm_hoversel_item_add(hoversel, "Idle", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "Happy", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "Speak", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "Wrong", NULL, ELM_ICON_NONE, NULL, NULL);
    evas_object_move(hoversel, WIDTH, HEIGHT/2);
    evas_object_resize(hoversel, 500, 40);
    evas_object_show(hoversel);
}

//State Based Animation (Advanced)
int demo1_1State = 2;
static void demo1_1MouseMoveCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)event_info;

   int posx = ev->cur.canvas.x;
   int posy = ev->cur.canvas.y;

   static bool preIn = false;
   static bool isIn = false;

   if (posx > 300 && posy > 300 && posx < 400 && posy < 400)
   {
      isIn = true;
      if (preIn == isIn) return;
      preIn = isIn;
      demo1_1State = 0;
      enableAnimation[demo1_1State] = true;
      enableAnimation[1] = false;
      enableAnimation[2] = false;
   }
   else
   {
      isIn = false;
      preIn = false;
      demo1_1State = 2;
      enableAnimation[demo1_1State] = true;
      enableAnimation[0] = false;
      enableAnimation[1] = false;
   }
}

static void mouseUpCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;

   int up_x = ev->canvas.x;
   int up_y = ev->canvas.y;

   for (int i=0; i < artboard->animationCount(); i++)
   {
      enableAnimation[i] = false;
      delete animationInstance[i];
      animationInstance[i] = nullptr;
      auto animation = artboard->animation(i);
      if (animation) animationInstance[i] = new rive::LinearAnimationInstance(animation);
   }

   if (up_x > 300 && up_y > 300 && up_x < 400 && up_y < 400)
   {
     demo1_1State = 1;
     enableAnimation[demo1_1State] = true;
   }
}

static void demo1_1()
{
    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_MOVE, demo1_1MouseMoveCb, nullptr);
    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_UP, mouseUpCb, nullptr);
}

//Animation Mixing
static void animationChangedCb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    int animationIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
    Eina_Bool bEnable = elm_check_state_get(obj);
    enableAnimation[animationIndex] = bEnable;
}

static void demo2()
{
    Eo* ch1 = elm_check_add(win);
    elm_object_text_set(ch1, "idle");
    evas_object_smart_callback_add(ch1, "changed", animationChangedCb, (void*)0);
    evas_object_show(ch1);
    evas_object_move(ch1, WIDTH, HEIGHT/2 + 100);
    evas_object_resize(ch1, 200, 20);
    infoWidgets.push_back(ch1);

    Eo* ch2 = elm_check_add(win);
    elm_object_text_set(ch2, "bouncing");
    evas_object_smart_callback_add(ch2, "changed", animationChangedCb, (void*)1);
    evas_object_show(ch2);
    evas_object_move(ch2, WIDTH, HEIGHT/2 + 120);
    evas_object_resize(ch2, 200, 20);
    infoWidgets.push_back(ch2);

    Eo* ch3 = elm_check_add(win);
    elm_object_text_set(ch3, "windshield_wipers");
    evas_object_smart_callback_add(ch3, "changed", animationChangedCb, (void*)2);
    evas_object_show(ch3);
    evas_object_move(ch3, WIDTH, HEIGHT/2 + 140);
    evas_object_resize(ch3, 200, 20);
    infoWidgets.push_back(ch3);

    Eo* ch4 = elm_check_add(win);
    elm_object_text_set(ch4, "broken");
    evas_object_smart_callback_add(ch4, "changed", animationChangedCb, (void*)3);
    evas_object_show(ch4);
    evas_object_move(ch4, WIDTH, HEIGHT/2 + 160);
    evas_object_resize(ch4, 200, 20);
    infoWidgets.push_back(ch4);
}


//Runtime Property Change
std::string currentColorInstance;
float r, g, b, a;
Eo *red, *green, *blue, *alpha;
static void selectedCb(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   const char *text = elm_object_item_text_get((Elm_Object_Item*)event_info);
   currentColorInstance = text;
   auto colorInstance = artboard->find<rive::Fill>(currentColorInstance.c_str());
   if (colorInstance)
   {
      int value = colorInstance->paint()->as<rive::SolidColor>()->colorValue();
      r = value >> 16 & 255;
      g = value >> 8 & 255;
      b = value >> 0 & 255;
      a = value >> 24 & 255;
      elm_slider_value_set(red, r);
      elm_slider_value_set(green, g);
      elm_slider_value_set(blue, b);
      elm_slider_value_set(alpha, a);
   }
}

void colorChangedCb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   int demoIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
   if (demoIndex == 0)
     r = elm_slider_value_get(obj);
   else if (demoIndex == 1)
     g = elm_slider_value_get(obj);
   else if (demoIndex == 2)
     b = elm_slider_value_get(obj);
   else if (demoIndex == 3)
     a = elm_slider_value_get(obj);

   auto colorInstance = artboard->find<rive::Fill>(currentColorInstance.c_str());
   if (colorInstance)
     colorInstance->paint()->as<rive::SolidColor>()->colorValue(rive::colorARGB(a, r, g, b));
}

static void demo4()
{
    Eo* hoversel = elm_hoversel_add(win);
    elm_hoversel_auto_update_set(hoversel, EINA_TRUE);
    elm_hoversel_hover_parent_set(hoversel, win);
    evas_object_smart_callback_add(hoversel, "selected", selectedCb, NULL);
    evas_object_size_hint_weight_set(hoversel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(hoversel, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_text_set(hoversel, "Vector Instances");
    elm_hoversel_item_add(hoversel, "body_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "straw_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "eye_left_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "eye_right_color", NULL, ELM_ICON_NONE, NULL, NULL);
    elm_hoversel_item_add(hoversel, "mouse_color", NULL, ELM_ICON_NONE, NULL, NULL);
    evas_object_move(hoversel, WIDTH, HEIGHT/2);
    evas_object_resize(hoversel, 500, 40);
    evas_object_show(hoversel);

    red = elm_slider_add(win);
    elm_slider_unit_format_set(red, "%d");
    elm_slider_indicator_format_set(red, "%0.1f");
    elm_slider_min_max_set(red, 0.0, 255.0);
    elm_object_text_set(red, "R");
    elm_slider_step_set(red, 1.0);
    evas_object_size_hint_align_set(red, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(red, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_slider_value_set(red, 0.0);
    evas_object_smart_callback_add(red, "changed", colorChangedCb, (void*)0);
    evas_object_move(red, WIDTH, HEIGHT/2 + 100);
    evas_object_resize(red, 400, 20);
    evas_object_show(red);

    green = elm_slider_add(win);
    elm_slider_unit_format_set(green, "%d");
    elm_slider_indicator_format_set(green, "%0.1f");
    elm_slider_min_max_set(green, 0.0, 255.0);
    elm_object_text_set(green, "G");
    elm_slider_step_set(green, 1.0);
    evas_object_size_hint_align_set(green, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(green, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_slider_value_set(green, 0.0);
    evas_object_smart_callback_add(green, "changed", colorChangedCb, (void*)1);
    evas_object_move(green, WIDTH, HEIGHT/2 + 120);
    evas_object_resize(green, 400, 20);
    evas_object_show(green);

    blue = elm_slider_add(win);
    elm_slider_unit_format_set(blue, "%d");
    elm_slider_indicator_format_set(blue, "%0.1f");
    elm_slider_min_max_set(blue, 0.0, 255.0);
    elm_object_text_set(blue, "B");
    elm_slider_step_set(blue, 1.0);
    evas_object_size_hint_align_set(blue, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(blue, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_slider_value_set(blue, 0.0);
    evas_object_smart_callback_add(blue, "changed", colorChangedCb, (void*)2);
    evas_object_move(blue, WIDTH, HEIGHT/2 + 140);
    evas_object_resize(blue, 400, 20);
    evas_object_show(blue);

    alpha = elm_slider_add(win);
    elm_slider_unit_format_set(alpha, "%d");
    elm_slider_indicator_format_set(alpha, "%0.1f");
    elm_slider_min_max_set(alpha, 0.0, 255.0);
    elm_object_text_set(alpha, "A");
    elm_slider_step_set(alpha, 1.0);
    evas_object_size_hint_align_set(alpha, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(alpha, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_slider_value_set(alpha, 0.0);
    evas_object_smart_callback_add(alpha, "changed", colorChangedCb, (void*)3);
    evas_object_move(alpha, WIDTH, HEIGHT/2 + 160);
    evas_object_resize(alpha, 400, 20);
    evas_object_show(alpha);

    infoWidgets.push_back(hoversel);
    infoWidgets.push_back(red);
    infoWidgets.push_back(green);
    infoWidgets.push_back(blue);
    infoWidgets.push_back(alpha);
}
//User Interaction RollInOout
static void mouseInOutCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)event_info;
   static bool preIn = false;
   static bool isIn = false;

   int viewx, viewy;
   evas_object_geometry_get(obj, &viewx, &viewy, nullptr, nullptr);

   int posx = ev->cur.canvas.x - viewx;
   int posy = ev->cur.canvas.y - viewy;

   // View Bounds
   if (posx > 100 && posy > 100 && posx < 620 && posy < 620)
   {
      isIn = true;
   }
   else
   {
      isIn = false;
   }

   if (preIn != isIn)
   {
      delete animationInstance[1];
      animationInstance[1] = nullptr;

      preIn = isIn;
      if (preIn)
      {
         auto animation = artboard->animation(1);
         if (animation) animationInstance[1] = new rive::LinearAnimationInstance(animation);
      }
      else
      {
         auto animation = artboard->animation(2);
         if (animation) animationInstance[1] = new rive::LinearAnimationInstance(animation);
      }
   }

   enableAnimation[1] = true;
}

static void demo3()
{
    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_MOVE, mouseInOutCb, nullptr);
}


//User Interaction Follow Curser
static void mouseMoveCb(void *data, Evas *evas EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)event_info;

   int viewx, viewy;
   evas_object_geometry_get(obj, &viewx, &viewy, nullptr, nullptr);

   // Viewx and viewy are the view start position
   int posx = ev->cur.canvas.x - viewx + 100;
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

static void demo5()
{
    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_MOVE, mouseMoveCb, nullptr);
}

//User Interaction View Transition
Eo* slider;
bool bTransitionDemo = false;
float transitionValue = 0.0;

void transitionChangedCb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
    transitionValue = elm_slider_value_get(obj);
    if (transitionValue == 1.0)
    {
      enableAnimation[2] = true;
      enableAnimation[3] = true;
    }
}

static void demo6()
{
    slider = elm_slider_add(win);
    elm_slider_unit_format_set(slider, "%f");
    elm_slider_indicator_format_set(slider, "%0.1f");
    elm_slider_min_max_set(slider, 0.0, 1.0);
    elm_object_text_set(slider, "time");
    elm_slider_step_set(slider, 0.1);
    evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_slider_value_set(slider, 0.0);
    evas_object_smart_callback_add(slider, "changed", transitionChangedCb, (void*)0);
    evas_object_move(slider, WIDTH, HEIGHT/2 + 100);
    evas_object_resize(slider, 400, 20);
    evas_object_show(slider);
    infoWidgets.push_back(slider);
}

static void fileClickedCb (void *data, Evas_Object *obj, void *event_info)
{
    int demoIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
    std::string path = RIVE_FILE_DIR;

    for (int i=0; i < infoWidgets.size(); i++)
    {
       if (infoWidgets[i])
       {
          evas_object_del(infoWidgets[i]);
       }
    }

    infoWidgets.clear();
    evas_object_event_callback_del_full(view, EVAS_CALLBACK_MOUSE_MOVE, mouseMoveCb, nullptr);
    evas_object_event_callback_del_full(view, EVAS_CALLBACK_MOUSE_MOVE, mouseInOutCb, nullptr);
    evas_object_event_callback_del_full(view, EVAS_CALLBACK_MOUSE_MOVE, demo1_1MouseMoveCb, nullptr);
    evas_object_event_callback_del_full(view, EVAS_CALLBACK_MOUSE_UP, mouseUpCb, nullptr);
    bTransitionDemo = false;

    for (int i=0; i < 30; i++)
    {
       enableAnimation[i] = false;
    }

    switch (demoIndex)
    {
       case 0:
       path.append("mr-help.riv");
       loadRiveFile(path.c_str());
       enableAnimation[3] = true;
       demo1();
       break;
       case 1:
       path.append("publish-icon.riv");
       loadRiveFile(path.c_str());
       enableAnimation[2] = true;
       demo1_1();
       break;
       case 2:
       path.append("buggy.riv");
       loadRiveFile(path.c_str());
       demo2();
       break;
       case 3:
       path.append("teeny_tiny_file.riv");
       loadRiveFile(path.c_str());
       enableAnimation[0] = true;
       demo3();
       break;
       case 4:
       path.append("runtime_color_change.riv");
       loadRiveFile(path.c_str());
       enableAnimation[0] = true;
       demo4();
       break;
       case 5:
       path.append("flame-and-spark.riv");
       loadRiveFile(path.c_str());
       enableAnimation[0] = true;
       demo5();
       break;
       case 6:
       path.append("space_reload.riv");
       loadRiveFile(path.c_str());
       enableAnimation[0] = true;
       enableAnimation[1] = true;
       demo6();
       bTransitionDemo = true;
       break;
    }
}

static std::vector<std::string> riveFiles(const std::string &dirName)
{
    DIR *d;
    struct dirent *dir;
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

    std::sort(result.begin(), result.end(), [](auto & a, auto &b){return a < b;});

    return result;
}

Eina_Bool animationLoop(void *data)
{
    canvas->clear();

    double currentTime = ecore_time_get();
    float elapsed = currentTime - lastTime;
    lastTime = currentTime;

    if (!artboard) return ECORE_CALLBACK_RENEW;

    for (int i = 0; i < artboard->animationCount(); i ++)
    {
        if (!animationInstance[i])
        {
            return ECORE_CALLBACK_RENEW;
        }
    }

    if (!bTransitionDemo)
    {
       for (int i = 0; i < artboard->animationCount(); i++)
       {
          if (enableAnimation[i])
          {
             animationInstance[i]->advance(elapsed);
             animationInstance[i]->apply(artboard);
             //printf("KTH %f \n", animationInstance[i]->time());
          }
       }
    }
    else
    {
       for (int i = 0; i < artboard->animationCount(); i++)
       {
          if (enableAnimation[i])
          {
             if (i == 1)
               animationInstance[i]->time(transitionValue);
             else
             {
               animationInstance[i]->advance(elapsed);
             }
             animationInstance[i]->apply(artboard);
             //printf("KTH %f \n", animationInstance[i]->time());
          }
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
    //Create a Canvas
    canvas = tvg::SwCanvas::gen();
    canvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);
    animator = ecore_animator_add(animationLoop, nullptr);
}

static void cleanExample()
{
    for (int i = 0; i < artboard->animationCount(); i ++)
    {
        if (animationInstance[i])
        {
            delete animationInstance[i];
            animationInstance[i] = nullptr;
        }
    }
}

static void animPopupItemCb(void *data EINA_UNUSED, Evas_Object *obj, void *event_info)
{
    int animationIndex = static_cast<int>(reinterpret_cast<intptr_t>(data));
//    initAnimation(animationIndex);
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

static void setupScreen(uint32_t* buffer)
{
    win = elm_win_util_standard_add(nullptr, "Rive Viewer");
    evas_object_smart_callback_add(win, "delete,request", deleteWindow, 0);

    view = evas_object_image_filled_add(evas_object_evas_get(win));
    evas_object_image_size_set(view, WIDTH, HEIGHT);
    evas_object_image_data_set(view, buffer);
    evas_object_image_pixels_get_callback_set(view, drawToCanvas, nullptr);
    evas_object_image_pixels_dirty_set(view, EINA_TRUE);
    evas_object_image_data_update_add(view, 0, 0, WIDTH, HEIGHT);
    evas_object_size_hint_weight_set(view, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_min_set(view, WIDTH, HEIGHT);
    evas_object_resize(view, WIDTH, HEIGHT);
    evas_object_move(view, 0, 0);
    evas_object_show(view);

    evas_object_event_callback_add(view, EVAS_CALLBACK_MOUSE_UP, viewClickedCb, nullptr);

    Eo *fileList = elm_list_add(win);
    evas_object_size_hint_weight_set(fileList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fileList, EVAS_HINT_FILL, EVAS_HINT_FILL);

/*
    // Search Rive Files in Resource Dir
    rivefiles = riveFiles(RIVE_FILE_DIR);
    for (size_t i = 0; i < rivefiles.size(); i++)
    {
       const char *ptr = strrchr(rivefiles[i].c_str(), '/');
       elm_list_item_append(fileList, ptr + 1, nullptr, nullptr, fileClickedCb, nullptr);
    }
*/
    elm_list_item_append(fileList, "State Based Animation", nullptr, nullptr, fileClickedCb, (void*)0);
    elm_list_item_append(fileList, "State Based Animation(Advanced)", nullptr, nullptr, fileClickedCb, (void*)1);
    elm_list_item_append(fileList, "Animation Mixing", nullptr, nullptr, fileClickedCb, (void*)2);
    elm_list_item_append(fileList, "Bone Based Animation", nullptr, nullptr, fileClickedCb, (void*)3);
    elm_list_item_append(fileList, "Runtime Color Change", nullptr, nullptr, fileClickedCb, (void*)4);
    elm_list_item_append(fileList, "Runtime Position Change", nullptr, nullptr, fileClickedCb, (void*)5);
    elm_list_item_append(fileList, "View Transition", nullptr, nullptr, fileClickedCb, (void*)6);
    elm_list_go(fileList);

    evas_object_resize(fileList, INFOWIDTH, HEIGHT/2);
    evas_object_show(fileList);
    evas_object_move(fileList, WIDTH, 0);

    evas_object_resize(win, WIDTH + INFOWIDTH , HEIGHT);
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
