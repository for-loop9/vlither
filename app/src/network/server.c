#include "server.h"

#include "../user.h"
#include "callback.h"

void server_connect(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;
  user_settings* usrs = &usr->usrs;

  char url[256] = {};
  sprintf(url, "ws://%s/slither", usrs->ipv4);
  mg_log_set(MG_LL_NONE);
  mg_mgr_init(&gdata->network_manager);
  gdata->connection =
      mg_ws_connect(&gdata->network_manager, url, server_callback, env,
                    "%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n",
                    "Accept-Encoding", "gzip, deflate", "Accept-Language",
                    "en-US,en;q=0.5", "Cache-Control", "no-cache", "DNT", "1",
                    "Origin", "https://slither.com", "Pragma", "no-cache");
}

void server_poll(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;

  mg_mgr_poll(&gdata->network_manager, 0);
}

void server_disconnect(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;

  mg_mgr_free(&gdata->network_manager);
}