#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

typedef struct {
  bool initialized;
  ma_engine engine;
  ma_sound button_sound;
  bool button_sound_initialized;
  ma_sound boost_sound;
  bool boost_sound_initialized;
  bool boost_sound_active;
  ma_sound bloop_sounds[16];
  bool bloop_sounds_initialized[16];
  ma_sound food_sounds[81];
  bool food_sounds_initialized[81];
  uint64_t food_last_eat_ms;
  int food_index;
} audio_state_t;

static audio_state_t g_audio_state = {0};

static void audio_shutdown_sound(ma_sound* sound) {
  if (!sound) return;
  if (g_audio_state.initialized) {
    ma_sound_stop(sound);
    ma_sound_uninit(sound);
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

void audio_set_boost_enabled(bool enabled) {
  if (!g_audio_state.initialized) {
    audio_init();
    if (!g_audio_state.initialized) return;
  }

  if (enabled) {
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
