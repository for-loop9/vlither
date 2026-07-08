#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

void audio_init(void);
void audio_shutdown(void);
void audio_apply_volume_settings(int master_percent, int eating_percent,
                                 int bloops_percent, int boosting_percent,
                                 int menu_percent);
void audio_set_boost_enabled(bool enabled);
void audio_play_button_click(void);
void audio_play_death_bloop_for_score(int score);
void audio_play_food_eat(void);
bool audio_button(const char* label, ImVec2 size);

#endif
