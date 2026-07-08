#include "user_settings.h"

#include <string.h>
#include <thermite.h>

void user_settings_default(user_settings* usr_settings) {
  usr_settings->ui_font_size = FONT_SIZE_SMALL;
  usr_settings->lb_font_size = FONT_SIZE_REGULAR;
  usr_settings->snake_names_font_size = FONT_SIZE_REGULAR;
  usr_settings->stats_font_size = FONT_SIZE_REGULAR;

  strcpy(usr_settings->version, SETTINGS_VERSION);

  usr_settings->bd_color[0] = 1;
  usr_settings->bd_color[1] = 0.25f;
  usr_settings->bd_color[2] = 0.25f;
  usr_settings->laser_color[0] = 0.5f;
  usr_settings->laser_color[1] = 1;
  usr_settings->laser_color[2] = 0.5f;
  usr_settings->laser_color[3] = 1;
  usr_settings->laser_thickness = 2;
  usr_settings->cursor_size = 48;
  usr_settings->minimap_size = 300;
  usr_settings->zoom_step = 0.1f;
  usr_settings->snake_scores = true;
  usr_settings->restart_rc = false;
  usr_settings->quit_mc = false;
  usr_settings->smooth_zoom = false;
  usr_settings->vsync = false;
  usr_settings->instant_restart = false;
  usr_settings->bot_radius_mult = 20;
  usr_settings->bot_follow_circle_score = 2000;
  usr_settings->sound_master_volume = 100;
  usr_settings->sound_eating_volume = 100;
  usr_settings->sound_bloops_volume = 100;
  usr_settings->sound_boosting_volume = 100;
  usr_settings->sound_menu_volume = 100;
  usr_settings->new_songs = true;
  usr_settings->music_volume = 100;

  // normal mode
  usr_settings->modes[0].food_flicker = true;
  usr_settings->modes[0].food_float = true;
  usr_settings->modes[0].uniform_food_color = false;
  usr_settings->modes[0].food_type = 0;
  usr_settings->modes[0].food_scale = 1;
  usr_settings->modes[0].food_color[0] = 1;
  usr_settings->modes[0].food_color[1] = 1;
  usr_settings->modes[0].food_color[2] = 1;
  usr_settings->modes[0].boost_type = 0;
  usr_settings->modes[0].qsm = 1;
  usr_settings->modes[0].bg_scale = 599 / 4096.0f;
  usr_settings->modes[0].boost_strength = 1;
  usr_settings->modes[0].show_crosshair = false;
  usr_settings->modes[0].show_boost = true;
  usr_settings->modes[0].show_shadows = true;
  usr_settings->modes[0].show_background = true;
  usr_settings->modes[0].show_accessories = true;
  usr_settings->modes[0].death_effect = true;
  usr_settings->modes[0].player_names_outline = false;
  usr_settings->modes[0].render_mode = 0;
  usr_settings->modes[0].const_food_scale = false;

  // assist mode
  usr_settings->modes[1].food_flicker = false;
  usr_settings->modes[1].food_float = false;
  usr_settings->modes[1].uniform_food_color = true;
  usr_settings->modes[1].food_type = 1;
  usr_settings->modes[1].food_scale = 1;
  usr_settings->modes[1].food_color[0] = 0.7f;
  usr_settings->modes[1].food_color[1] = 0.7f;
  usr_settings->modes[1].food_color[2] = 0.7f;
  usr_settings->modes[1].boost_type = 1;
  usr_settings->modes[1].qsm = 1;
  usr_settings->modes[1].bg_scale = 599 / 4096.0f;
  usr_settings->modes[1].boost_strength = 1;
  usr_settings->modes[1].show_crosshair = true;
  usr_settings->modes[1].show_boost = false;
  usr_settings->modes[1].show_shadows = true;
  usr_settings->modes[1].show_background = false;
  usr_settings->modes[1].show_accessories = false;
  usr_settings->modes[1].death_effect = false;
  usr_settings->modes[1].player_names_outline = true;
  usr_settings->modes[1].render_mode = 1;
  usr_settings->modes[1].const_food_scale = false;

  usr_settings->hotkeys[HOTKEY_HUD] = (hotkey){GLFW_KEY_H, true, 0, "HUD"};
  usr_settings->hotkeys[HOTKEY_SHOW_NAMES] =
      (hotkey){GLFW_KEY_P, true, 0, "Show names"};
  usr_settings->hotkeys[HOTKEY_BIG_FOOD] =
      (hotkey){GLFW_KEY_F, false, 0, "Big food"};
  usr_settings->hotkeys[HOTKEY_ASSIST] =
      (hotkey){GLFW_KEY_K, false, 1, "Assist"};
  usr_settings->hotkeys[HOTKEY_BOT] =
      (hotkey){GLFW_KEY_T, false, 0, "Bot"};
  usr_settings->hotkeys[HOTKEY_MENU] =
      (hotkey){GLFW_KEY_Z, true, 0, "Hotkey menu"};
  usr_settings->hotkeys[HOTKEY_RESTART] =
      (hotkey){GLFW_KEY_R, false, 1, "Restart"};
  usr_settings->hotkeys[HOTKEY_QUIT] = (hotkey){GLFW_KEY_Q, false, 1, "Quit"};
}

void write_default_settings(user_settings* usr_settings) {
  user_settings_default(usr_settings);

  FILE* f = fopen(USER_SETTINGS_FILE, "wb");
  if (f == NULL) {
    printf("Error creating settings file.");
    exit(-1);
  }

  fwrite(usr_settings, sizeof(user_settings), 1, f);
  fclose(f);
}

void read_user_settings(user_settings* usr_settings) {
  FILE* f = fopen(USER_SETTINGS_FILE, "rb");

  if (f == NULL) {
    // File doesn't exist → create it
    write_default_settings(usr_settings);
    return;
  }

  size_t read = fread(usr_settings, sizeof(user_settings), 1, f);
  fclose(f);

  if (read != 1 || strncmp(usr_settings->version, SETTINGS_VERSION,
                           strlen(SETTINGS_VERSION)) != 0) {
    printf("Settings file outdated, recreating with default settings.\n");
    write_default_settings(usr_settings);
    return;
  }
}

void save_user_settings(user_settings* usr_settings) {
  FILE* f = fopen(USER_SETTINGS_FILE, "wb");
  if (f == NULL) {
    printf("Error saving settings.");
    exit(-1);
  }

  fwrite(usr_settings, sizeof(user_settings), 1, f);
  fclose(f);
}
