#include "audio.h"

#include "constants.h"
#include "game/game_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

typedef enum {
  AUDIO_MENU_PHASE_NONE = 0,
  AUDIO_MENU_PHASE_MENU1 = 1,
  AUDIO_MENU_PHASE_MENU2 = 2,
} audio_menu_phase_t;

typedef struct {
  bool initialized;
  ma_engine engine;
  ma_sound button_sound;
  bool button_sound_initialized;
  ma_sound boost_sound;
  bool boost_sound_initialized;
  bool boost_sound_active;
  ma_sound music_sound;
  bool music_sound_initialized;
  bool music_active;
  bool music_in_menu;
  audio_menu_phase_t music_menu_phase;
  bool music_use_new_songs;
  bool music_pending_transition;
  int music_volume_percent;
  int music_current_screen;
  int music_gameplay_track;
  float master_volume;
  float eating_volume;
  float bloops_volume;
  float boosting_volume;
  float menu_volume;
  ma_sound bloop_sounds[16];
  bool bloop_sounds_initialized[16];
  ma_sound food_sounds[81];
  bool food_sounds_initialized[81];
  uint64_t food_last_eat_ms;
  int food_index;
} audio_state_t;

static audio_state_t g_audio_state = {0};

static float audio_percent_to_volume(int percent) {
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent == 0 ? 0.0f : (float)percent / 100.0f;
}

static void audio_set_sound_volume(ma_sound* sound, float volume) {
  if (sound) {
    ma_sound_set_volume(sound, volume);
  }
}

static void audio_refresh_existing_sound_volumes(void) {
  float master = g_audio_state.master_volume;
  float eating = g_audio_state.eating_volume;
  float bloops = g_audio_state.bloops_volume;
  float boosting = g_audio_state.boosting_volume;
  float menu = g_audio_state.menu_volume;

  audio_set_sound_volume(&g_audio_state.button_sound, master * menu);
  audio_set_sound_volume(&g_audio_state.boost_sound, master * boosting);

  for (int i = 0; i < 16; ++i) {
    if (g_audio_state.bloop_sounds_initialized[i]) {
      audio_set_sound_volume(&g_audio_state.bloop_sounds[i], master * bloops);
    }
  }
  for (int i = 1; i <= 80; ++i) {
    if (g_audio_state.food_sounds_initialized[i]) {
      audio_set_sound_volume(&g_audio_state.food_sounds[i], master * eating);
    }
  }
}

static void audio_shutdown_sound(ma_sound* sound) {
  if (!sound) return;
  if (g_audio_state.initialized) {
    ma_sound_stop(sound);
    ma_sound_uninit(sound);
  }
}

static void audio_music_advance(void);

static void audio_music_end_callback(void* pUserData, ma_sound* pSound) {
  (void)pUserData;
  (void)pSound;
  g_audio_state.music_pending_transition = true;
}

static void audio_stop_music(void) {
  if (!g_audio_state.initialized) return;
  if (g_audio_state.music_sound_initialized) {
    ma_sound_stop(&g_audio_state.music_sound);
    ma_sound_uninit(&g_audio_state.music_sound);
    g_audio_state.music_sound_initialized = false;
  }
  g_audio_state.music_active = false;
}

static void audio_start_music_track(const char* path, bool looping, bool chain_callback) {
  if (!g_audio_state.initialized) return;

  if (g_audio_state.music_sound_initialized) {
    ma_sound_stop(&g_audio_state.music_sound);
    ma_sound_uninit(&g_audio_state.music_sound);
  }

  ma_result result = ma_sound_init_from_file(&g_audio_state.engine, path, 0, NULL,
                                             NULL, &g_audio_state.music_sound);
  if (result != MA_SUCCESS) {
    return;
  }

  g_audio_state.music_sound_initialized = true;
  g_audio_state.music_active = true;
  ma_sound_set_looping(&g_audio_state.music_sound, looping);
  ma_sound_set_volume(&g_audio_state.music_sound,
                      audio_percent_to_volume(g_audio_state.music_volume_percent));
  if (chain_callback) {
    ma_sound_set_end_callback(&g_audio_state.music_sound,
                              audio_music_end_callback, NULL);
  } else {
    ma_sound_set_end_callback(&g_audio_state.music_sound, NULL, NULL);
  }
  ma_sound_start(&g_audio_state.music_sound);
}

static void audio_start_menu_music(void) {
  if (!g_audio_state.initialized) return;

  g_audio_state.music_in_menu = true;
  g_audio_state.music_menu_phase = AUDIO_MENU_PHASE_MENU1;
  g_audio_state.music_use_new_songs = false;
  audio_start_music_track("app/res/sounds/songs/menu1.wav", false, true);
}

static void audio_start_gameplay_music(void) {
  if (!g_audio_state.initialized) return;

  g_audio_state.music_in_menu = false;
  g_audio_state.music_menu_phase = AUDIO_MENU_PHASE_NONE;
  if (g_audio_state.music_use_new_songs) {
    g_audio_state.music_gameplay_track = rand() % 8 + 1;
    char path[64] = {0};
    snprintf(path, sizeof(path), "app/res/sounds/songs/%d.wav",
             g_audio_state.music_gameplay_track);
    audio_start_music_track(path, false, true);
  } else {
    audio_start_music_track("app/res/sounds/songs/game.wav", true, false);
  }
}

static void audio_music_advance(void) {
  if (!g_audio_state.initialized || !g_audio_state.music_active) return;

  if (g_audio_state.music_in_menu) {
    g_audio_state.music_menu_phase = AUDIO_MENU_PHASE_MENU2;
    audio_start_music_track("app/res/sounds/songs/menu2.wav", true, false);
    g_audio_state.music_in_menu = true;
    return;
  }

  if (g_audio_state.music_use_new_songs) {
    int next_track = g_audio_state.music_gameplay_track % 8 + 1;
    g_audio_state.music_gameplay_track = next_track;
    char path[64] = {0};
    snprintf(path, sizeof(path), "app/res/sounds/songs/%d.wav", next_track);
    audio_start_music_track(path, false, true);
  }
}

void audio_update(void) {
  if (!g_audio_state.initialized) return;

  if (g_audio_state.music_pending_transition) {
    g_audio_state.music_pending_transition = false;
    audio_music_advance();
  }
}

void audio_init(void) {
  if (g_audio_state.initialized) return;

  ma_engine_config engine_config = ma_engine_config_init();
  if (ma_engine_init(&engine_config, &g_audio_state.engine) != MA_SUCCESS) {
    return;
  }

  g_audio_state.initialized = true;
  g_audio_state.button_sound_initialized = false;
  g_audio_state.boost_sound_initialized = false;
  g_audio_state.boost_sound_active = false;
  g_audio_state.master_volume = 1.0f;
  g_audio_state.eating_volume = 1.0f;
  g_audio_state.bloops_volume = 1.0f;
  g_audio_state.boosting_volume = 1.0f;
  g_audio_state.menu_volume = 1.0f;
}

void audio_shutdown(void) {
  if (!g_audio_state.initialized) return;

  audio_shutdown_sound(&g_audio_state.button_sound);
  if (g_audio_state.boost_sound_initialized) {
    audio_shutdown_sound(&g_audio_state.boost_sound);
    g_audio_state.boost_sound_initialized = false;
  }
  g_audio_state.boost_sound_active = false;
  for (int i = 0; i < 16; ++i) {
    if (g_audio_state.bloop_sounds_initialized[i]) {
      audio_shutdown_sound(&g_audio_state.bloop_sounds[i]);
      g_audio_state.bloop_sounds_initialized[i] = false;
    }
  }
  for (int i = 1; i <= 80; ++i) {
    if (g_audio_state.food_sounds_initialized[i]) {
      audio_shutdown_sound(&g_audio_state.food_sounds[i]);
      g_audio_state.food_sounds_initialized[i] = false;
    }
  }
  ma_engine_uninit(&g_audio_state.engine);
  memset(&g_audio_state, 0, sizeof(g_audio_state));
}

void audio_apply_volume_settings(int master_percent, int eating_percent,
                                 int bloops_percent, int boosting_percent,
                                 int menu_percent) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  g_audio_state.master_volume = audio_percent_to_volume(master_percent);
  g_audio_state.eating_volume = audio_percent_to_volume(eating_percent);
  g_audio_state.bloops_volume = audio_percent_to_volume(bloops_percent);
  g_audio_state.boosting_volume = audio_percent_to_volume(boosting_percent);
  g_audio_state.menu_volume = audio_percent_to_volume(menu_percent);
  audio_refresh_existing_sound_volumes();
}

void audio_apply_music_settings(bool new_songs_enabled, int volume_percent) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  g_audio_state.music_use_new_songs = new_songs_enabled;
  g_audio_state.music_volume_percent = volume_percent;
  if (g_audio_state.music_sound_initialized) {
    ma_sound_set_volume(&g_audio_state.music_sound,
                        audio_percent_to_volume(volume_percent) *
                            g_audio_state.master_volume);
  }
}

void audio_update_music_for_screen(int screen) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  if (screen == TITLE_SCREEN || screen == SETTINGS || screen == SKIN_EDITOR) {
    if (!g_audio_state.music_active || !g_audio_state.music_in_menu) {
      audio_start_menu_music();
    }
    return;
  }

  if (screen == PLAYING) {
    audio_start_gameplay_music();
    return;
  }

  audio_stop_music();
}

void audio_set_boost_enabled(bool enabled) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  if (enabled) {
    if (g_audio_state.master_volume <= 0.0f ||
        g_audio_state.boosting_volume <= 0.0f) {
      return;
    }
    if (!g_audio_state.boost_sound_initialized) {
      ma_result result = ma_sound_init_from_file(&g_audio_state.engine,
                                                 "app/res/sounds/boost.wav",
                                                 0,
                                                 NULL,
                                                 NULL,
                                                 &g_audio_state.boost_sound);
      if (result != MA_SUCCESS) {
        return;
      }

      ma_sound_set_looping(&g_audio_state.boost_sound, true);
      audio_set_sound_volume(&g_audio_state.boost_sound,
                             g_audio_state.master_volume *
                                 g_audio_state.boosting_volume);
      g_audio_state.boost_sound_initialized = true;
    }

    if (!g_audio_state.boost_sound_active) {
      ma_sound_seek_to_pcm_frame(&g_audio_state.boost_sound, 0);
      ma_sound_start(&g_audio_state.boost_sound);
      g_audio_state.boost_sound_active = true;
    }
    return;
  }

  if (g_audio_state.boost_sound_initialized && g_audio_state.boost_sound_active) {
    ma_sound_stop(&g_audio_state.boost_sound);
    g_audio_state.boost_sound_active = false;
  }
}

void audio_play_button_click(void) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  if (!g_audio_state.button_sound_initialized) {
    ma_result result = ma_sound_init_from_file(&g_audio_state.engine,
                                               "app/res/sounds/button.wav",
                                               0,
                                               NULL,
                                               NULL,
                                               &g_audio_state.button_sound);
    if (result != MA_SUCCESS) {
      return;
    }

    g_audio_state.button_sound_initialized = true;
  }

  if (g_audio_state.master_volume <= 0.0f ||
      g_audio_state.menu_volume <= 0.0f) {
    return;
  }

  audio_set_sound_volume(&g_audio_state.button_sound,
                         g_audio_state.master_volume *
                             g_audio_state.menu_volume);
  ma_sound_seek_to_pcm_frame(&g_audio_state.button_sound, 0);
  ma_sound_start(&g_audio_state.button_sound);
}

void audio_play_death_bloop_for_score(int score) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  int clamped_score = score < 0 ? 0 : score;
  int bloop_index = 15 - (clamped_score / 100);
  if (bloop_index < 0) bloop_index = 0;
  if (bloop_index > 15) bloop_index = 15;

  if (!g_audio_state.bloop_sounds_initialized[bloop_index]) {
    char bloop_path[64] = {0};
    snprintf(bloop_path, sizeof(bloop_path), "app/res/sounds/bloops/%d.wav", bloop_index);
    ma_result result = ma_sound_init_from_file(&g_audio_state.engine,
                                               bloop_path,
                                               0,
                                               NULL,
                                               NULL,
                                               &g_audio_state.bloop_sounds[bloop_index]);
    if (result != MA_SUCCESS) {
      return;
    }

    g_audio_state.bloop_sounds_initialized[bloop_index] = true;
  }

  if (g_audio_state.master_volume <= 0.0f ||
      g_audio_state.bloops_volume <= 0.0f) {
    return;
  }

  audio_set_sound_volume(&g_audio_state.bloop_sounds[bloop_index],
                         g_audio_state.master_volume *
                             g_audio_state.bloops_volume);
  ma_sound_seek_to_pcm_frame(&g_audio_state.bloop_sounds[bloop_index], 0);
  ma_sound_start(&g_audio_state.bloop_sounds[bloop_index]);
}

void audio_play_food_eat(void) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  uint64_t now_ms = GetTickCount64();
  if (!g_audio_state.food_last_eat_ms || now_ms - g_audio_state.food_last_eat_ms > 100) {
    g_audio_state.food_index = 1;
  } else if (g_audio_state.food_index < 80) {
    g_audio_state.food_index++;
  } else {
    g_audio_state.food_index = 80;
  }
  g_audio_state.food_last_eat_ms = now_ms;

  int food_index = g_audio_state.food_index;
  if (!g_audio_state.food_sounds_initialized[food_index]) {
    char food_path[64] = {0};
    snprintf(food_path, sizeof(food_path), "app/res/sounds/foods/%d.wav", food_index);
    ma_result result = ma_sound_init_from_file(&g_audio_state.engine,
                                               food_path,
                                               0,
                                               NULL,
                                               NULL,
                                               &g_audio_state.food_sounds[food_index]);
    if (result != MA_SUCCESS) {
      return;
    }
    g_audio_state.food_sounds_initialized[food_index] = true;
  }

  if (g_audio_state.master_volume <= 0.0f ||
      g_audio_state.eating_volume <= 0.0f) {
    return;
  }

  audio_set_sound_volume(&g_audio_state.food_sounds[food_index],
                         g_audio_state.master_volume *
                             g_audio_state.eating_volume);
  ma_sound_seek_to_pcm_frame(&g_audio_state.food_sounds[food_index], 0);
  ma_sound_start(&g_audio_state.food_sounds[food_index]);
}

bool audio_button(const char* label, ImVec2 size) {
  bool pressed = igButton(label, size);
  if (pressed) {
    audio_play_button_click();
  }
  return pressed;
}
