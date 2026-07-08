#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

void audio_init(void);
void audio_shutdown(void);
void audio_play_button_click(void);
void audio_play_death_bloop_for_score(int score);
bool audio_button(const char* label, ImVec2 size);

#endif
