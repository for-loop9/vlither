#include "ig_mouse.h"
#include "ig_window.h"

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	user_data* ud = glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
		ig_darray_push(&ud->mouse->buttons_pressed, &button);
	else if (action == GLFW_RELEASE)
		ig_darray_push(&ud->mouse->buttons_released, &button);
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
	user_data* ud = glfwGetWindowUserPointer(window);
	ud->mouse->pos.x = x;
	ud->mouse->pos.y = y;
}

void mouse_wheel_callback(GLFWwindow* window, double x, double y) {
	user_data* ud = glfwGetWindowUserPointer(window);
	ud->mouse->dwheel = (float) y;
}

ig_mouse* ig_mouse_create(ig_window* window) {
	glfwSetMouseButtonCallback(window->native_handle, mouse_button_callback);
	glfwSetCursorPosCallback(window->native_handle, mouse_pos_callback);
	glfwSetScrollCallback(window->native_handle, mouse_wheel_callback);

	ig_mouse* r = malloc(sizeof(ig_mouse));
	r->buttons_pressed = ig_darray_create(int);
	r->buttons_released = ig_darray_create(int);
	r->pos = (ig_vec2) { .x = 0, .y = 0 };
	r->prev_pos = (ig_vec2) { .x = 0, .y = 0 };
	r->delta = (ig_vec2) { .x = 0, .y = 0 };
	r->dwheel = 0;
	window->ud.mouse = r;

	return r;
}

void ig_mouse_update(ig_mouse* mouse) {
	ig_darray_clear(mouse->buttons_pressed);
	ig_darray_clear(mouse->buttons_released);

	ig_vec2_sub(&mouse->delta, &mouse->pos, &mouse->prev_pos);
	mouse->prev_pos = mouse->pos;
	mouse->dwheel = 0;
}

int ig_mouse_button_pressed(ig_mouse* mouse, int button) {
	int tot_btns = ig_darray_length(mouse->buttons_pressed);
	for (int i = 0; i < tot_btns; i++) {
		if (button == mouse->buttons_pressed[button])
			return 1;
	}
	return 0;
}

int ig_mouse_button_released(ig_mouse* mouse, int button) {
	int tot_btns = ig_darray_length(mouse->buttons_released);
	for (int i = 0; i < tot_btns; i++) {
		if (button == mouse->buttons_released[button])
			return 1;
	}
	return 0;
}

void ig_mouse_destroy(ig_mouse* mouse) {
	ig_darray_destroy(mouse->buttons_released);
	ig_darray_destroy(mouse->buttons_pressed);
	free(mouse);
}