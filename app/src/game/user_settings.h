#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include "../constants.h"
#include <stdint.h>
#include <stdbool.h>
#include <cglm/cglm.h>

typedef struct user_settings {
  char nickname[MAX_NICKNAME_LEN + 1];
  char ipv4[MAX_IPV4_LEN + 1];
  char skin_code[MAX_SKIN_CODE_LEN + 1];
  bool custom_skin;
  uint8_t default_skin;
  int score;
  double play_time;
  int kills;
  font_size ui_font_size;
  font_size lb_font_size;
  font_size snake_names_font_size;
  font_size stats_font_size;

  vec3 bd_color;
  vec3 food_color;

  int cursor_size;
  int minimap_size;
  
  bool food_flicker;
  bool food_float;
  int food_type;
  bool boost_effect;
  bool snake_scores;
  bool restart_rc;
  bool quit_mc;
  bool smooth_zoom;

  float food_scale;
  float qsm;
  float zoom_step;
  float bg_scale;
  bool uniform_food_color;

  struct {
    bool shadow;
    bool boost;
    bool hud;
    bool background;
    bool big_food;
    bool peek_names;
    bool toggle_hotkeys;
    bool fullscreen;
    bool crosshair;
  } hotkeys;
} user_settings;

void user_settings_default(user_settings* usr_settings);
void read_user_settings(user_settings* usr_settings);
void save_user_settings(user_settings* usr_settings);

#endif