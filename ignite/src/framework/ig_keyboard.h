#ifndef IG_KEYBOARD_H
#define IG_KEYBOARD_H

#include "../memory/ig_darray.h"

typedef struct ig_window ig_window;

typedef struct ig_keyboard {
	int* keys_pressed;
	int* keys_released;
	char char_pressed;
} ig_keyboard;

ig_keyboard* ig_keyboard_create(ig_window* window);
void ig_keyboard_update(ig_keyboard* keyboard);
int ig_keyboard_key_pressed(ig_keyboard* keyboard, int key);
int ig_keyboard_key_released(ig_keyboard* keyboard, int key);
void ig_keyboard_destroy(ig_keyboard* keyboard);

#endif