#include "settings.h"

#include "../user.h"

void ui_settings_init(tenv* env) {}

void ui_settings(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  user_settings* usrs = &usr->usrs;
  ImGuiStyle* style = igGetStyle();
  ImGuiIO* io = igGetIO_Nil();
  game_data* gdata = &usr->gdata;

  igPushFont(usr->imgui_data.regular_font[usrs->ui_font_size],
             usr->imgui_data.regular_font[usrs->ui_font_size]->LegacySize);

  float frame_height = igGetFrameHeight();
  float child_window_height =
      ctx->size[1] - style->WindowPadding.y * 4 - frame_height;

  if (igBeginTable("settings_table", 4, ImGuiTableFlags_None, (ImVec2){}, 0)) {
    igTableNextRow(ImGuiTableRowFlags_None, 0);
    igTableSetColumnIndex(0);

    igBeginChild_Str("general_settings_child_holder",
                     (ImVec2){-1, child_window_height}, ImGuiChildFlags_None,
                     ImGuiWindowFlags_None);
    igSeparatorText("General");
    if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
      igTableNextRow(ImGuiTableRowFlags_None, 0);
      igTableSetColumnIndex(0);
      igIndent(style->WindowPadding.x);
      igAlignTextToFramePadding();
      igText("VSync");
      igAlignTextToFramePadding();
      igText("Cursor size");
      igAlignTextToFramePadding();
      igText("UI font size");
      igAlignTextToFramePadding();
      igText("Stats font size");
      igAlignTextToFramePadding();
      igText("Leaderboard font size");
      igAlignTextToFramePadding();
      igText("Names font size");
      igAlignTextToFramePadding();
      igText("Show snake scores");
      igAlignTextToFramePadding();
      igText("Smooth zoom");
      igAlignTextToFramePadding();
      igText("Zoom step");
      igAlignTextToFramePadding();
      igText("Border color");
      igAlignTextToFramePadding();
      igText("Minimap size");
      igAlignTextToFramePadding();
      igText("Instant restart");
      igAlignTextToFramePadding();
      igText("Restart with right click");
      igAlignTextToFramePadding();
      igText("Quit with middle click");
      igAlignTextToFramePadding();
      igText("Laser color");
      igAlignTextToFramePadding();
      igText("Laser thickness");
      igAlignTextToFramePadding();
      igText("Bot circle after score");
      igAlignTextToFramePadding();
      igText("Bot radius multiplier");

      igTableSetColumnIndex(1);
      if (igCheckbox("##vsync", &usrs->vsync)) {
        env->config.vsync = usrs->vsync;
        twindow_request_refresh(env->wnd);
      }
      igSetNextItemWidth(-1);
      igSliderInt("##cursor size", &usrs->cursor_size, 16, 64, "%d px",
                  ImGuiSliderFlags_AlwaysClamp);
      igSetNextItemWidth(-1);
      igCombo_Str_arr("##ui font size", (int*)&usrs->ui_font_size,
                      (const char*[]){"Small", "Regular", "Large"}, 3, -1);
      igSetNextItemWidth(-1);
      igCombo_Str_arr("##stats font size", (int*)&usrs->stats_font_size,
                      (const char*[]){"Small", "Regular", "Large"}, 3, -1);
      igSetNextItemWidth(-1);
      igCombo_Str_arr("##leaderboard font size", (int*)&usrs->lb_font_size,
                      (const char*[]){"Small", "Regular", "Large"}, 3, -1);
      igSetNextItemWidth(-1);
      igCombo_Str_arr("##snake name font size",
                      (int*)&usrs->snake_names_font_size,
                      (const char*[]){"Small", "Regular", "Large"}, 3, -1);
      igCheckbox("##snake scores", &usrs->snake_scores);
      igCheckbox("##smooth zoom", &usrs->smooth_zoom);
      igSetNextItemWidth(-1);
      igSliderFloat("##zoom step", &usrs->zoom_step, 0.05f, 0.5f, "%.2f",
                    ImGuiSliderFlags_AlwaysClamp);
      igSetNextItemWidth(-1);
      igColorEdit3("##border color", usrs->bd_color, ImGuiColorEditFlags_None);
      igSetNextItemWidth(-1);
      igSliderInt("##minimap size", &usrs->minimap_size, 128, 512, "%d px",
                  ImGuiSliderFlags_AlwaysClamp);
      igCheckbox("##instant restart", &usrs->instant_restart);
      igCheckbox("##restart rc", &usrs->restart_rc);
      igCheckbox("##quit mc", &usrs->quit_mc);
      igSetNextItemWidth(-1);
      igColorEdit4("##laser color", usrs->laser_color,
                   ImGuiColorEditFlags_AlphaBar);
      igSetNextItemWidth(-1);
      igSliderInt("##laser thickness", &usrs->laser_thickness, 1, 4, "%d px",
                  ImGuiSliderFlags_AlwaysClamp);
                  igSetNextItemWidth(-1);
      igSliderInt("##circle after", &usrs->bot_follow_circle_score, 1000, 6000, "%d",
                  ImGuiSliderFlags_AlwaysClamp);
                  igSetNextItemWidth(-1);
      igSliderInt("##rad mult", &usrs->bot_radius_mult, 10, 40, "%dx",
                  ImGuiSliderFlags_AlwaysClamp);
      igIndent(-style->WindowPadding.x);
      igEndTable();
    }
    igEndChild();

    for (int i = 0; i < 2; i++) {
      igTableSetColumnIndex(i + 1);
      igPushID_Int(i + 1);
      gameplay_mode* mode = usrs->modes + i;
      igBeginChild_ID(igGetID_Int(i + 1), (ImVec2){-1, child_window_height},
                      ImGuiChildFlags_None, ImGuiWindowFlags_None);
      igSeparatorText(i == 0 ? "Normal mode" : "Assist mode");

      if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
        igTableNextRow(ImGuiTableRowFlags_None, 0);
        igTableSetColumnIndex(0);
        igIndent(style->WindowPadding.x);
        igAlignTextToFramePadding();
        igText("Show crosshair");
        igAlignTextToFramePadding();
        igText("Show background");
        igAlignTextToFramePadding();
        igText("Show accessories");
        igAlignTextToFramePadding();
        igText("Show shadows");
        igAlignTextToFramePadding();
        igText("Death effect");
        igAlignTextToFramePadding();
        igText("Outline player names");
        igAlignTextToFramePadding();
        igText("Segment separation");
        igAlignTextToFramePadding();
        igText("Background scale");
        igAlignTextToFramePadding();
        igText("Render mode");
        igAlignTextToFramePadding();
        igText("Boost effect");
        igAlignTextToFramePadding();
        igText("Boost effect strength");
        igAlignTextToFramePadding();
        igText("Food shader");
        igAlignTextToFramePadding();
        igText("Food scale");
        igAlignTextToFramePadding();
        igText("Food float");
        igAlignTextToFramePadding();
        igText("Food flicker");
        igAlignTextToFramePadding();
        igText("Uniform food color");

        igTableSetColumnIndex(1);
        igCheckbox("##crosshair", &mode->show_crosshair);
        igCheckbox("##bg", &mode->show_background);
        igCheckbox("##acc", &mode->show_accessories);
        igCheckbox("##shad", &mode->show_shadows);
        igCheckbox("##death effect", &mode->death_effect);
        igCheckbox("##player names outline", &mode->player_names_outline);
        igSetNextItemWidth(-1);
        igSliderFloat("##bps", &mode->qsm, 1, 4, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igSetNextItemWidth(-1);
        igSliderFloat("##bgs", &mode->bg_scale, 0.05, 4, "%.2fx",
                      ImGuiSliderFlags_AlwaysClamp);
        igSetNextItemWidth(-1);
        igCombo_Str_arr("##render mode", &mode->render_mode,
                        (const char*[]){"Texture", "Solid", "Flat"}, 3, -1);

        igCheckbox("##boost", &mode->show_boost);
        igSameLine(0, -1);
        igBeginDisabled(!mode->show_boost);
        igSetNextItemWidth(-1);
        igCombo_Str_arr("##boost type", &mode->boost_type,
                        (const char*[]){"Normal", "Simple"}, 2, -1);
        igSetNextItemWidth(-1);
        igSliderFloat("##boost strength", &mode->boost_strength, 0.25f, 3,
                      "%.2fx", ImGuiSliderFlags_AlwaysClamp);
        igEndDisabled();
        igSetNextItemWidth(-1);
        igCombo_Str_arr("##food type", &mode->food_type,
                        (const char*[]){"Solid", "Rings"}, 2, -1);
        igSetNextItemWidth(-1);
        igSliderFloat("##food scale", &mode->food_scale, 0.25f, 3, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igCheckbox("##food float", &mode->food_float);
        igCheckbox("##food flicker", &mode->food_flicker);
        igCheckbox("##uniform food color", &mode->uniform_food_color);
        igSameLine(0, -1);
        igBeginDisabled(!mode->uniform_food_color);
        igSetNextItemWidth(-1);
        igColorEdit3("##fdcolor", mode->food_color, ImGuiColorEditFlags_None);
        igEndDisabled();
        igIndent(-style->WindowPadding.x);

        igEndTable();
      }
      igEndChild();
      igPopID();
    }

    igTableSetColumnIndex(3);
    igBeginChild_Str("hotkey_child_window", (ImVec2){-1, child_window_height},
                     ImGuiChildFlags_None, ImGuiWindowFlags_None);
    igSeparatorText("Hotkeys");
    if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
      igTableNextRow(ImGuiTableRowFlags_None, 0);
      igTableSetColumnIndex(0);
      igIndent(style->WindowPadding.x);
      for (int i = 0; i < NUM_HOTKEYS; i++) {
        hotkey* hk = usrs->hotkeys + i;
        igAlignTextToFramePadding();
        igText(hk->description);
      }
      igTableSetColumnIndex(1);

      for (int i = 0; i < NUM_HOTKEYS; i++) {
        hotkey* hk = usrs->hotkeys + i;
        igPushID_Int(i);
        igSetNextItemWidth(frame_height * 2);
        char preview_char[2] = {(char)hk->key, 0};
        if (igBeginCombo("##hotkey code", preview_char, ImGuiComboFlags_None)) {
          for (int c = 48; c < 58; c++) {
            char selectable_char[2] = {c, 0};
            bool is_in_use = false;
            for (int d = 0; d < NUM_HOTKEYS; d++) {
              if (c == usrs->hotkeys[d].key &&
                  hk->key != usrs->hotkeys[d].key) {
                is_in_use = true;
              }
            }
            if (igSelectable_Bool(selectable_char, c == hk->key,
                                  is_in_use ? ImGuiSelectableFlags_Disabled
                                            : ImGuiSelectableFlags_None,
                                  (ImVec2){})) {
              hk->key = c;
            }
          }
          for (int c = 65; c < 91; c++) {
            char selectable_char[2] = {c, 0};
            bool is_in_use = false;
            for (int d = 0; d < NUM_HOTKEYS; d++) {
              if (c == usrs->hotkeys[d].key &&
                  hk->key != usrs->hotkeys[d].key) {
                is_in_use = true;
              }
            }
            is_in_use = is_in_use || c == GLFW_KEY_M || c == GLFW_KEY_N;
            if (igSelectable_Bool(selectable_char, c == hk->key,
                                  is_in_use ? ImGuiSelectableFlags_Disabled
                                            : ImGuiSelectableFlags_None,
                                  (ImVec2){})) {
              hk->key = c;
            }
          }
          igEndCombo();
        }
        igSameLine(0, -1);
        ImVec2 rest;
        igGetContentRegionAvail(&rest);
        igSetNextItemWidth(rest.x - style->ItemInnerSpacing.x);
        if (i == HOTKEY_RESTART || i == HOTKEY_QUIT) {
          igBeginDisabled(true);
          igCombo_Str_arr("##hotkey mode", &(int){0}, (const char*[]){"Toggle"},
                          1, -1);
          igEndDisabled();
        } else {
          igCombo_Str_arr("##hotkey mode", &hk->mode,
                          (const char*[]){"Toggle", "Press and hold"}, 2, -1);
        }
        igPopID();
      }
      igIndent(-style->WindowPadding.x);

      igEndTable();
    }
    igEndChild();

    igEndTable();
  }

  igSetCursorPosX(ctx->size[0] - style->WindowPadding.x - 150 -
                  style->ItemSpacing.x - 150);
  igSetCursorPosY(ctx->size[1] - style->WindowPadding.y - frame_height);
  if (igButton("Reset", (ImVec2){150, 0})) {
    user_settings_default(usrs);
    env->config.vsync = usrs->vsync;
    twindow_request_refresh(env->wnd);
  }
  igSameLine(0, -1);
  if (igButton("OK", (ImVec2){150, 0})) {
    save_user_settings(usrs);
    gdata->curr_screen = TITLE_SCREEN;
  }

  igPopFont();
}

void ui_settings_destroy(tenv* env) {}
