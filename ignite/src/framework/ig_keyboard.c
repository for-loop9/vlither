#include "ig_keyboard.h"
#include "ig_window.h"
#include <stdio.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	user_data* ud = glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
		ig_darray_push(&ud->keyboard->keys_pressed, &key);
	else if (action == GLFW_RELEASE)
		ig_darray_push(&ud->keyboard->keys_released, &key);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
	user_data* ud = glfwGetWindowUserPointer(window);
	ud->keyboard->char_pressed = (char) codepoint;
}

ig_keyboard* ig_keyboard_create(ig_window* window) {
	glfwSetKeyCallback(window->native_handle, key_callback);
	glfwSetCharCallback(window->native_handle, char_callback);

	ig_keyboard* r = malloc(sizeof(ig_keyboard));
	r->keys_pressed = ig_darray_create(int);
	r->keys_released = ig_darray_create(int);
	window->ud.keyboard = r;

	return r;
}

void ig_keyboard_update(ig_keyboard* keyboard) {
	ig_darray_clear(keyboard->keys_pressed);
	ig_darray_clear(keyboard->keys_released);
	keyboard->char_pressed = 0;
}

int ig_keyboard_key_pressed(ig_keyboard* keyboard, int key) {
	return ig_darray_find(keyboard->keys_pressed, &key) != -1;
}

int ig_keyboard_key_released(ig_keyboard* keyboard, int key) {
	return ig_darray_find(keyboard->keys_released, &key) != -1;
}

void ig_keyboard_destroy(ig_keyboard* keyboard) {
	ig_darray_destroy(keyboard->keys_released);
	ig_darray_destroy(keyboard->keys_pressed);
	free(keyboard);
}
