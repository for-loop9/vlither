#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include "../constants.h"
#include <stdint.h>
#include <stdbool.h>
#include <cglm/cglm.h>

typedef struct gameplay_mode {
  bool food_flicker;
  bool food_float;
  bool uniform_food_color;
  vec3 food_color;
  int food_type;
  float food_scale;
  float qsm;
  float bg_scale;
  int boost_type;
  bool show_boost;
  bool show_shadows;
  bool show_background;
  bool show_accessories;
  int render_mode;
} gameplay_mode;

typedef struct user_settings {
  char nickname[MAX_NICKNAME_LEN + 1];
  char ipv4[MAX_IPV4_LEN + 1];
  char skin_code[MAX_SKIN_CODE_LEN + 1];
  uint8_t accessory;
  bool custom_skin;
  uint8_t default_skin;
  int score;
  double play_time;
  int kills;
  font_size ui_font_size;
  font_size lb_font_size;
  font_size snake_names_font_size;
  font_size stats_font_size;

  // global settings:
  vec3 bd_color;
  vec4 laser_color;
  int laser_thickness;
  int cursor_size;
  int minimap_size;
  bool restart_rc;
  bool quit_mc;
  bool vsync;
  bool smooth_zoom;
  bool snake_scores;
  float zoom_step;

  gameplay_mode modes[2];

  // hotkeys:
  struct {
    bool fullscreen;
    bool hud;
    bool big_food;
    bool show_names;
    bool crosshair;
    bool assist;
    bool toggle_hotkeys;
  } hotkeys;
} user_settings;

void user_settings_default(user_settings* usr_settings);
void read_user_settings(user_settings* usr_settings);
void save_user_settings(user_settings* usr_settings);

#endif