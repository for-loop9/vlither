#include "user_settings.h"

#include <string.h>

void user_settings_default(user_settings* usr_settings) {
  usr_settings->ui_font_size = FONT_SIZE_SMALL;
  usr_settings->lb_font_size = FONT_SIZE_REGULAR;
  usr_settings->snake_names_font_size = FONT_SIZE_REGULAR;
  usr_settings->stats_font_size = FONT_SIZE_REGULAR;

  usr_settings->bd_color[0] = 1;
  usr_settings->bd_color[1] = 0.25f;
  usr_settings->bd_color[2] = 0.25f;
  usr_settings->food_color[0] = 1;
  usr_settings->food_color[1] = 1;
  usr_settings->food_color[2] = 1;

  usr_settings->cursor_size = 48;
  usr_settings->minimap_size = 300;

  usr_settings->uniform_food_color = false;
  usr_settings->food_flicker = true;
  usr_settings->food_float = true;
  usr_settings->food_scale = 1;
  usr_settings->food_type = 0;
  usr_settings->boost_effect = 0;
  usr_settings->snake_scores = true;
  usr_settings->restart_rc = true;
  usr_settings->quit_mc = true;
  usr_settings->qsm = 1;
  usr_settings->smooth_zoom = false;
  usr_settings->zoom_step = 0.1f;
  usr_settings->bg_scale = 599 / 4096.0f;

  usr_settings->hotkeys.big_food = false;
  usr_settings->hotkeys.background = true;
  usr_settings->hotkeys.shadow = true;
  usr_settings->hotkeys.boost = true;
  usr_settings->hotkeys.hud = true;
  usr_settings->hotkeys.peek_names = true;
  usr_settings->hotkeys.toggle_hotkeys = true;
  usr_settings->hotkeys.fullscreen = false;
  usr_settings->hotkeys.crosshair = true;
}

void read_user_settings(user_settings* usr_settings) {
  FILE* f = fopen(USER_SETTINGS_FILE, "rb");
  if (f == NULL) {
    user_settings_default(usr_settings);
    f = fopen(USER_SETTINGS_FILE, "wb");
    if (f == NULL) {
      printf("Error creating settings file.");
      exit(-1);
    }

    fwrite(usr_settings, sizeof(user_settings), 1, f);
    fclose(f);
    return;
  }

  size_t read = fread(usr_settings, sizeof(user_settings), 1, f);
  fclose(f);

  if (read != 1) {
    printf("Settings file corrupted.");
    exit(-1);
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
