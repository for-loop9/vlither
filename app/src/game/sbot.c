#include "sbot.h"

#include "../user.h"

void sbot_init(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;
  sbot* bot = &gdata->bot;

  bot->output.accel = 0;
  bot->output.xm = 0;
  bot->output.ym = 0;
}

void sbot_go(tenv* env) {}

void sbot_destroy(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;
  sbot* bot = &gdata->bot;
}
