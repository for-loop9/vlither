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

  if (igBeginTable("settings_table", 4, ImGuiTableFlags_None, (ImVec2){}, 0)) {
    igTableNextRow(ImGuiTableRowFlags_None, 0);
    igTableSetColumnIndex(0);

    igBeginChild_Str("settings_child_holder", (ImVec2){-1, -1}, ImGuiChildFlags_None, ImGuiWindowFlags_None);
    if (igCollapsingHeader_BoolPtr("General", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
      if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
        igTableNextRow(ImGuiTableRowFlags_None, 0);
        igTableSetColumnIndex(0);
        igIndent(style->WindowPadding.x);
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
        igText("Body parts separation");
        igAlignTextToFramePadding();
        igText("Background scale");
        igAlignTextToFramePadding();
        igText("Smooth zoom");
        igAlignTextToFramePadding();
        igText("Zoom step");
        igAlignTextToFramePadding();
        igText("Border color");
        igAlignTextToFramePadding();
        igText("Minimap size");
        igAlignTextToFramePadding();
        igText("Restart with right click");
        igAlignTextToFramePadding();
        igText("Quit with middle click");

        igTableSetColumnIndex(1);
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
        igSetNextItemWidth(-1);
        igSliderFloat("##bps", &usrs->qsm, 1, 4, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igSetNextItemWidth(-1);
        igSliderFloat("##bg scale", &usrs->bg_scale, 0.05f, 4, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igCheckbox("##smooth zoom", &usrs->smooth_zoom);
        igSetNextItemWidth(-1);
        igSliderFloat("##zoom step", &usrs->zoom_step, 0.05f, 0.5f, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igSetNextItemWidth(-1);
        igColorEdit3("##border color", usrs->bd_color,
                     ImGuiColorEditFlags_None);
        igSetNextItemWidth(-1);
        igSliderInt("##minimap size", &usrs->minimap_size, 128, 512, "%d px",
                    ImGuiSliderFlags_AlwaysClamp);
        igCheckbox("##restart rc", &usrs->restart_rc);
        igCheckbox("##quit mc", &usrs->quit_mc);
        igIndent(-style->WindowPadding.x);
        igEndTable();
      }
    }
    if (igCollapsingHeader_BoolPtr("Food", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
      if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
        igTableNextRow(ImGuiTableRowFlags_None, 0);
        igTableSetColumnIndex(0);
        igIndent(style->WindowPadding.x);
        igAlignTextToFramePadding();
        igText("Shader");
        igAlignTextToFramePadding();
        igText("Scale");
        igAlignTextToFramePadding();
        igText("Float");
        igAlignTextToFramePadding();
        igText("Flicker");
        igAlignTextToFramePadding();
        igText("Uniform color");

        igTableSetColumnIndex(1);
        igSetNextItemWidth(-1);
        igCombo_Str_arr("##food type", &usrs->food_type,
                        (const char*[]){"Solid", "Rings"}, 2, -1);
        igSetNextItemWidth(-1);
        igSliderFloat("##food scale", &usrs->food_scale, 0.25f, 3, "%.2f",
                      ImGuiSliderFlags_AlwaysClamp);
        igCheckbox("##food float", &usrs->food_float);
        igCheckbox("##food flicker", &usrs->food_flicker);
        igCheckbox("##uniform food color", &usrs->uniform_food_color);
        igSameLine(0, -1);
        igBeginDisabled(!usrs->uniform_food_color);
        igSetNextItemWidth(-1);
        igColorEdit3("##fdcolor", usrs->food_color, ImGuiColorEditFlags_None);
        igEndDisabled();
        igIndent(-style->WindowPadding.x);

        igEndTable();
      }
    }
    if (igCollapsingHeader_BoolPtr("Boost", NULL, ImGuiTreeNodeFlags_DefaultOpen)) {
      if (igBeginTable("field:value", 2, ImGuiTableFlags_None, (ImVec2){}, 0)) {
        igTableNextRow(ImGuiTableRowFlags_None, 0);
        igTableSetColumnIndex(0);
        igIndent(style->WindowPadding.x);
        igAlignTextToFramePadding();
        igText("Boost type");

        igTableSetColumnIndex(1);
        igSetNextItemWidth(-1);
        int boost_type = usrs->boost_effect;
        igCombo_Str_arr("##boost type", &boost_type,
                        (const char*[]){"Normal", "Simple"}, 2, -1);
        usrs->boost_effect = boost_type;

        igIndent(-style->WindowPadding.x);
        igEndTable();
      }
    }

    igEndChild();
    igEndTable();
  }

  igSetCursorPosX(ctx->size[0] - style->WindowPadding.x - 150 - style->ItemSpacing.x - 150);
  igSetCursorPosY(ctx->size[1] - style->WindowPadding.y - igGetFrameHeight());
  if (igButton("Reset", (ImVec2){150, 0})) {
    user_settings_default(usrs);
  } igSameLine(0, -1); 
  if (igButton("OK", (ImVec2){150, 0})) {
    save_user_settings(usrs);
    gdata->curr_screen = TITLE_SCREEN;
  }

  igPopFont();
}

void ui_settings_destroy(tenv* env) {}
