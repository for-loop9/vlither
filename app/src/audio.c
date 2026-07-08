#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MA_IMPLEMENTATION
#include "external/miniaudio.h"

typedef struct {
  bool initialized;
  ma_engine engine;
  ma_sound button_sound;
  bool button_sound_initialized;
  ma_sound bloop_sounds[16];
  bool bloop_sounds_initialized[16];
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
}

void audio_shutdown(void) {
  if (!g_audio_state.initialized) return;

  audio_shutdown_sound(&g_audio_state.button_sound);
  for (int i = 0; i < 16; ++i) {
    if (g_audio_state.bloop_sounds_initialized[i]) {
      audio_shutdown_sound(&g_audio_state.bloop_sounds[i]);
      g_audio_state.bloop_sounds_initialized[i] = false;
    }
  }
  ma_engine_uninit(&g_audio_state.engine);
  memset(&g_audio_state, 0, sizeof(g_audio_state));
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

bool audio_button(const char* label, ImVec2 size) {
  bool pressed = igButton(label, size);
  if (pressed) {
    audio_play_button_click();
  }
  return pressed;
}
