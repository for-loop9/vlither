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
    int snakes_len = tdarray_length(gdata->data.snakes);
    snake* me = gdata->data.snakes + (snakes_len - 1);

    gdata->data.wmd = twindow_button_down(env->wnd, GLFW_MOUSE_BUTTON_LEFT) || twindow_key_down(env->wnd, BOOST_HKEY);

    if (gdata->data.md != gdata->data.wmd &&
        gdata->data.ctm - gdata->data.last_accel_mtm > 150) {
      gdata->data.md = gdata->data.wmd;
      gdata->data.last_accel_mtm = gdata->data.ctm;
      mg_ws_send(connection, (uint8_t[]){gdata->data.md ? 253 : 254}, 1,
                 WEBSOCKET_OP_BINARY);
    }

    int xm = (int)env->ms->pos[0] - ctx->size[0] / 2;
    int ym = (int)env->ms->pos[1] - ctx->size[1] / 2;

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

  gdata->data.gsc *= expf(env->ms->dwheel * usrs->zoom_step);

  if (tkeyboard_key_pressed(env->kb, ZOOM_IN_HKEY))
    gdata->data.gsc *= expf(1 * usrs->zoom_step);
  else if (tkeyboard_key_pressed(env->kb, ZOOM_OUT_HKEY))
    gdata->data.gsc *= expf(-1 * usrs->zoom_step);

  gdata->data.gsc = GLM_MAX(MAX_ZOOM_OUT, GLM_MIN(gdata->data.gsc, MAX_ZOOM_IN)),

  // hotkeys
  usrs->hotkeys.shadow ^= tkeyboard_key_pressed(env->kb, SHADOW_HKEY);
  usrs->hotkeys.boost ^= tkeyboard_key_pressed(env->kb, BOOST_EFFECT_HKEY);
  usrs->hotkeys.hud ^= tkeyboard_key_pressed(env->kb, HUD_HKEY);
  usrs->hotkeys.background ^= tkeyboard_key_pressed(env->kb, BACKGROUND_HKEY);
  usrs->hotkeys.big_food ^= tkeyboard_key_pressed(env->kb, BIG_FOOD_HKEY);
  usrs->hotkeys.peek_names ^= tkeyboard_key_pressed(env->kb, PEEK_NAMES_HKEY);
  usrs->hotkeys.toggle_hotkeys ^= tkeyboard_key_pressed(env->kb, HOTKEYS_HKEY);
}