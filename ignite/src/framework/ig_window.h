#ifndef IG_WINDOW_H
#define IG_WINDOW_H

#include <GLFW/glfw3.h>
#include "../math/ig_ivec2.h"

typedef struct ig_keyboard ig_keyboard;
typedef struct ig_mouse ig_mouse;

typedef struct user_data {
	ig_keyboard* keyboard;
	ig_mouse* mouse;
} user_data;

typedef struct ig_window {
	GLFWwindow* native_handle;
	ig_ivec2 dim;
	user_data ud;
} ig_window;

ig_window* ig_window_create_asp(float asp_ratio, const char* title);
ig_window* ig_window_create(const ig_ivec2* dim, const char* title, int full_screen, int monitor);

void ig_window_input(ig_window* window);
int ig_window_closed(ig_window* window);
int ig_window_keyboard_key_down(ig_window* window, int key);
int ig_window_mouse_button_down(ig_window* window, int button);

void ig_window_destroy(ig_window* window);

#endif