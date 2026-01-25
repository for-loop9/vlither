#ifndef USER_H
#define USER_H

#include "imgui_setup.h"
#include "rendering/renderer.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"
#include "constants.h"

#include "game/game_data.h"
#include "game/user_settings.h"

TDEF_USER_DATA({
  renderer* r;

  struct {
    ImFont* mono_font[NUM_FONT_SIZES];
    ImFont* regular_font[NUM_FONT_SIZES];
    ImFont* mono_font_bold[NUM_FONT_SIZES];
    ImFont* regular_font_bold[NUM_FONT_SIZES];
  } imgui_data;

  struct {
    VkDescriptorSet* scene;
  } viewport_widget;

  game_data gdata;
  user_settings usrs;
});

#endif