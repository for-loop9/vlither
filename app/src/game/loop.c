#include "loop.h"

#include "../network/server.h"
#include "../user.h"
#include "input.h"
#include "oef.h"
#include "redraw.h"
#include "ui_overlay.h"

void game_loop(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;
  user_settings* usrs = &usr->usrs;

  if (!env->config.running) gdata->conn = DISCONNECTED;

  switch (gdata->conn) {
    case CONNECTING: {
      usr->r->global.bg_opacity = 0;
      usr->r->global.bd_opacity = 0;
      usr->r->global.minimap_opacity = 0;

      if (glfwGetTime() > TIMEOUT) {
        gdata->connection->is_closing = true;
        printf("Connection timed out.");
      }

      server_poll(env);

      vec2 loading_bar = {500, 12};
      igSetCursorPosX(ctx->size[0] * 0.5f - loading_bar[0] * 0.5f);
      igSetCursorPosY(ctx->size[1] * 0.5f - loading_bar[1] * 0.5f);

      igPushStyleColor_Vec4(ImGuiCol_PlotHistogram,
                            (ImVec4){0.168f, 0.668f, 0.375f, 1});
      igProgressBar(-glfwGetTime(), (ImVec2){loading_bar[0], loading_bar[1]},
                    NULL);
      igPopStyleColor(1);

      if (gdata->closed) {
        gdata->conn = DISCONNECTED;
        gdata->closed = false;
      }
      break;
    }
    case CONNECTED:
      time_step(env);
      input(env);
      server_poll(env);
      oef(env);
      redraw(env);
      ui_overlay(env);

      // special hotkeys
      if (usrs->hotkeys[HOTKEY_QUIT].active ||
          (usrs->quit_mc &&
           tmouse_button_pressed(env->ms, GLFW_MOUSE_BUTTON_MIDDLE))) {
        gdata->connection->is_closing = true;
      } else if (usrs->hotkeys[HOTKEY_RESTART].active ||
                 (usrs->restart_rc &&
                  tmouse_button_pressed(env->ms, GLFW_MOUSE_BUTTON_RIGHT))) {
        gdata->connection->is_closing = true;
        gdata->restart_req = true;
      }

      if (gdata->closed) {
        game_data_reset(env);

        if (gdata->restart_req) {
          usr->gdata.conn = CONNECTING;
          glfwSetTime(0);
          server_connect(env);
          gdata->restart_req = false;
        } else {
          usr->gdata.conn = DISCONNECTED;
        }
        gdata->closed = false;
      }

      break;
    case DISCONNECTED:
      usr->r->global.bg_opacity = 0;
      usr->r->global.bd_opacity = 0;
      usr->r->global.minimap_opacity = 0;

      gdata->curr_screen = TITLE_SCREEN;

      game_data_reset(env);
      server_poll(env);

      break;
  }
}
