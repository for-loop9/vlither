#include "input.h"

#include "../user.h"

void input(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;
  user_settings* usrs = &usr->usrs;
  struct mg_connection* connection = gdata->connection;

  if (!gdata->data.wfpr) {
    if (gdata->data.ctm - gdata->data.last_ping_mtm > 250) {
      gdata->data.last_ping_mtm = gdata->data.ctm;
      gdata->data.wfpr = true;
      mg_ws_send(connection, (uint8_t[]){251}, 1, WEBSOCKET_OP_BINARY);
    }
  }

  if (gdata->data.follow_view) {
    int xm;
    int ym;

    int snakes_len = tdarray_length(gdata->data.snakes);
    snake* me = gdata->data.snakes + (snakes_len - 1);

    if (usrs->hotkeys[HOTKEY_BOT].active) {
      xm = gdata->bot.output.xm;
      ym = gdata->bot.output.ym;
    } else {
      if (twindow_key_down(env->wnd, GLFW_KEY_LEFT))
        gdata->data.kd_l_frb += gdata->data.vfrb;
      if (twindow_key_down(env->wnd, GLFW_KEY_RIGHT))
        gdata->data.kd_r_frb += gdata->data.vfrb;

      if (gdata->data.kd_l_frb > 0 || gdata->data.kd_r_frb > 0)
        if (gdata->data.ctm - gdata->data.lkstm > 150) {
          gdata->data.lkstm = gdata->data.ctm;
          if (gdata->data.kd_r_frb > 0)
            if (gdata->data.kd_l_frb > gdata->data.kd_r_frb) {
              gdata->data.kd_l_frb -= gdata->data.kd_r_frb;
              gdata->data.kd_r_frb = 0;
            }
          if (gdata->data.kd_l_frb > 0)
            if (gdata->data.kd_r_frb > gdata->data.kd_l_frb) {
              gdata->data.kd_r_frb -= gdata->data.kd_l_frb;
              gdata->data.kd_l_frb = 0;
            }
          if (gdata->data.kd_l_frb > 0) {
            int v = gdata->data.kd_l_frb;
            if (v > 127) v = 127;
            gdata->data.kd_l_frb -= v;
            me->eang -= gdata->data.mamu * v * me->scang * me->spang;
            mg_ws_send(connection, (uint8_t[]){252, (uint8_t)v}, 2,
                       WEBSOCKET_OP_BINARY);
          } else if (gdata->data.kd_r_frb > 0) {
            int v = gdata->data.kd_r_frb;
            if (v > 127) v = 127;
            gdata->data.kd_r_frb -= v;
            me->eang += gdata->data.mamu * v * me->scang * me->spang;
            v += 128;
            mg_ws_send(connection, (uint8_t[]){252, (uint8_t)v}, 2,
                       WEBSOCKET_OP_BINARY);
          }
        }

      xm = (int)env->ms->pos[0] - ctx->size[0] / 2;
      ym = (int)env->ms->pos[1] - ctx->size[1] / 2;
    }
    gdata->data.wmd = twindow_button_down(env->wnd, GLFW_MOUSE_BUTTON_LEFT) ||
                      twindow_key_down(env->wnd, GLFW_KEY_SPACE) ||
                      twindow_key_down(env->wnd, GLFW_KEY_UP) ||
                      gdata->bot.output.accel;

    if (gdata->data.md != gdata->data.wmd &&
        gdata->data.ctm - gdata->data.last_accel_mtm > 150) {
      gdata->data.md = gdata->data.wmd;
      gdata->data.last_accel_mtm = gdata->data.ctm;
      mg_ws_send(connection, (uint8_t[]){gdata->data.md ? 253 : 254}, 1,
                 WEBSOCKET_OP_BINARY);
    }

    bool want_e = false;
    if (xm != gdata->data.lsxm || ym != gdata->data.lsym) want_e = true;
    me->eang = atan2f(ym, xm);
    float ang;
    if (want_e && gdata->data.ctm - gdata->data.last_e_mtm > 50) {
      want_e = false;
      gdata->data.last_e_mtm = gdata->data.ctm;
      gdata->data.lsxm = xm;
      gdata->data.lsym = ym;
      float d2 = xm * xm + ym * ym;
      if (d2 > 256) {
        ang = atan2f(ym, xm);
        me->eang = ang;
      } else
        ang = me->wang;
      ang = fmodf(ang, PI2);
      if (ang < 0) ang += PI2;
      int sang = (int)floorf((250 + 1) * ang / PI2);
      if (sang != gdata->data.lsang) {
        gdata->data.lsang = sang;
        mg_ws_send(connection, (uint8_t[]){sang & 255}, 1, WEBSOCKET_OP_BINARY);
      }
    }
  }

  gdata->data.ms_zoom *= expf(env->ms->dwheel * usrs->zoom_step);

  if (tkeyboard_key_pressed(env->kb, GLFW_KEY_N))
    gdata->data.ms_zoom *= expf(1 * usrs->zoom_step);
  else if (tkeyboard_key_pressed(env->kb, GLFW_KEY_M))
    gdata->data.ms_zoom *= expf(-1 * usrs->zoom_step);

  gdata->data.ms_zoom =
      GLM_MAX(MAX_ZOOM_OUT, GLM_MIN(gdata->data.ms_zoom, MAX_ZOOM_IN));

  // hotkeys
  usrs->hotkeys[HOTKEY_RESTART].active = false;
  usrs->hotkeys[HOTKEY_QUIT].active = false;

  for (int i = 0; i < NUM_HOTKEYS; i++) {
    hotkey* hk = usrs->hotkeys + i;
    if (hk->mode)
      hk->active = twindow_key_down(env->wnd, hk->key);
    else
      hk->active ^= tkeyboard_key_pressed(env->kb, hk->key);
  }

  if (gdata->data.follow_view) {
    snake* me = gdata->data.snakes + (tdarray_length(gdata->data.snakes) - 1);
    int score = (int)floorf((gdata->data.fpsls[me->sct] +
                             me->fam / gdata->data.fmlts[me->sct] - 1) *
                                15 -
                            5) /
                1;
    if (score >= 1000) {
      usrs->hotkeys[HOTKEY_RESTART].active = false;
    }
  }

  gameplay_mode* mode = usrs->modes + usrs->hotkeys[HOTKEY_ASSIST].active;
  if (mode->show_crosshair) igSetMouseCursor(ImGuiMouseCursor_None);
}