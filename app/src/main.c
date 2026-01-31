#include "game/loop.h"
#include "ui/skin_editor.h"
#include "ui/title_screen.h"
#include "ui/settings.h"
#include "ui/viewport.h"
#include "user.h"

void tinput(tenv* env) {
  tuser_data* usr = env->usr;
  user_settings* usrs = &usr->usrs;

  if (twindow_closed(env->wnd)) {
    env->config.running = false;
    save_user_settings(usrs);
  }
  if (tkeyboard_key_pressed(env->kb, GLFW_KEY_F11)) {
    twindow_toggle_fullscreen(env->wnd);
  }
}

void tlaunch(tenv* env) {
  tuser_data* usr = env->usr;
  user_settings* usrs = &usr->usrs;
  srand(time(NULL));

  memset(usrs, 0, sizeof(user_settings));
  strcpy(usrs->ipv4, "15.204.212.200:444");
  strcpy(usrs->nickname, "");
  usrs->custom_skin = false;
  usrs->default_skin = rand() % 9;
  usrs->accessory = NO_ACCESSORY;

  read_user_settings(usrs);
  env->config.vsync = usrs->vsync;
  env->config.fullscreen = false;
  env->config.title = "Vlither";
}

void tinit(tenv* env) {
  tuser_data* usr = env->usr;

  imgui_init(env);
  env->usr->r = renderer_create(env);
  ui_viewport_init(env);
  ui_title_screen_init(env);
  ui_skin_editor_init(env);
  ui_settings_init(env);
  game_data_init(env);
}

void tdestroy(tenv* env) {
  game_data_destroy(env);
  ui_settings_destroy(env);
  ui_skin_editor_destroy(env);
  ui_title_screen_destroy(env);
  ui_viewport_destroy(env);
  renderer_destroy(env->usr->r, env->ctx);
  imgui_destroy();
}

void trender(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;

  imgui_prerender();
  // render begin
  ImGuiStyle* style = igGetStyle();
  ui_viewport(env);

  igSetNextWindowPos(igGetMainViewport()->Pos, ImGuiCond_None, (ImVec2){});
  igSetNextWindowSize(igGetMainViewport()->Size, ImGuiCond_None);
  igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0);
  igBegin("##fullscreen_holder", NULL,
          ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
              ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  igPopStyleVar(1);
  switch (usr->gdata.curr_screen) {
    case TITLE_SCREEN:
      ui_title_screen(env);
      break;
    case SKIN_EDITOR:
      ui_skin_editor(env);
      break;
    case PLAYING:
      game_loop(env);
      break;
    case SETTINGS:
      ui_settings(env);
      break;
  }
  igEnd();
  // render end

  igRender();
  if (tcontext_begin(ctx)) {
    renderer_render(usr->r, ctx, (vec4){0.086f, 0.109f, 0.133f, 1});
    tcontext_clear(ctx, (vec4){0, 0, 0, 1.0f});
    imgui_render(ctx->frames[ctx->current_frame].cmd);
    renderer_render_cursor(usr->r, ctx);
    tcontext_end(ctx);
  }
  renderer_clear_instances(usr->r);
}

void tresize(tenv* env) { ui_viewport_resize(env); }

TDEF_ENTRY();