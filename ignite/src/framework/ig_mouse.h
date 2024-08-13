#ifndef IG_MOUSE_H
#define IG_MOUSE_H

#include "../memory/ig_darray.h"
#include "../math/ig_vec2.h"

typedef struct ig_window ig_window;

typedef struct ig_mouse {
	int* buttons_pressed;
	int* buttons_released;

	ig_vec2 pos;
	ig_vec2 prev_pos;
	ig_vec2 delta;
	float dwheel;
} ig_mouse;

ig_mouse* ig_mouse_create(ig_window* window);
void ig_mouse_update(ig_mouse* mouse);
int ig_mouse_button_pressed(ig_mouse* mouse, int button);
int ig_mouse_button_released(ig_mouse* mouse, int button);
void ig_mouse_destroy(ig_mouse* mouse);

#endif