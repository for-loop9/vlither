#ifndef TMOUSE_H
#define TMOUSE_H

#include <cglm/struct.h>

typedef struct twindow twindow;

typedef struct tmouse {
  int* buttons_pressed;
  int* buttons_released;

  vec2 pos;
  vec2 prev_pos;
  vec2 delta;
  float dwheel;
} tmouse;

tmouse* tmouse_create(twindow* window);
void tmouse_update(tmouse* mouse);
int tmouse_button_pressed(tmouse* mouse, int button);
int tmouse_button_released(tmouse* mouse, int button);
void tmouse_destroy(tmouse* mouse);

#endif