#include "ig_window.h"
#include <stdlib.h>

ig_window* ig_window_create_asp(float asp_ratio, const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	ig_window* r = (ig_window*) malloc(sizeof(ig_window));
	const GLFWvidmode* vid_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int window_width = vid_mode->width * 0.75f;
    int window_height = window_width / asp_ratio;

    if (window_height > vid_mode->height * 0.75f) {
        window_height = vid_mode->height * 0.75f;
        window_width = window_height * asp_ratio;
    }

	r->dim.x = window_width;
	r->dim.y = window_height;

	r->native_handle = glfwCreateWindow(r->dim.x, r->dim.y, title, NULL, NULL);
	glfwSetWindowPos(r->native_handle, (vid_mode->width / 2) - (r->dim.x / 2), (vid_mode->height / 2) - (r->dim.y / 2));
	glfwSetWindowUserPointer(r->native_handle, &r->ud);

	return r;
}

ig_window* ig_window_create(const ig_ivec2* dim, const char* title, int full_screen, int monitor) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	ig_window* r = (ig_window*) malloc(sizeof(ig_window));
	int monitor_count;
	GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
	const GLFWvidmode* vid_mode = glfwGetVideoMode(monitors[monitor]);
	r->dim = full_screen ? (ig_ivec2) { .x = vid_mode->width, .y = vid_mode->height } : *dim;

	r->native_handle = glfwCreateWindow(r->dim.x, r->dim.y, title, full_screen ? monitors[monitor] : NULL, NULL);
	if (!full_screen)
		glfwSetWindowPos(r->native_handle, (vid_mode->width / 2) - (r->dim.x / 2), (vid_mode->height / 2) - (r->dim.y / 2));
	glfwSetWindowUserPointer(r->native_handle, &r->ud);

	return r;
}

void ig_window_input(ig_window* window) {
	glfwPollEvents();
}

int ig_window_closed(ig_window* window) {
	return glfwWindowShouldClose(window->native_handle);
}

int ig_window_keyboard_key_down(ig_window* window, int key) {
	return glfwGetKey(window->native_handle, key) == GLFW_PRESS;
}

int ig_window_mouse_button_down(ig_window* window, int button) {
	return glfwGetMouseButton(window->native_handle, button);
}

void ig_window_destroy(ig_window* window) {
	glfwDestroyWindow(window->native_handle);
	glfwTerminate();
	free(window);
}
