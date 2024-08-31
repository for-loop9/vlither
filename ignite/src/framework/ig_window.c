#include "ig_window.h"
#include <stdlib.h>
#include <stdio.h>

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	user_data* ud = glfwGetWindowUserPointer(window);
	ud->window->dim.x = width;
	ud->window->dim.y = height;
	ud->window->resize_requested = true;
}

ig_window* ig_window_create_asp(float asp_ratio, const char* title, const GLFWimage* icons, int icons_count) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	ig_window* r = (ig_window*) malloc(sizeof(ig_window));
	r->resize_requested = false;
	r->ud.window = r;
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int window_width = vidmode->width * 0.75f;
    int window_height = window_width / asp_ratio;

    if (window_height > vidmode->height * 0.75f) {
        window_height = vidmode->height * 0.75f;
        window_width = window_height * asp_ratio;
    }

	r->dim.x = window_width;
	r->dim.y = window_height;
	r->last_dim.x = window_width;
	r->last_dim.y = window_height;

	r->native_handle = glfwCreateWindow(r->dim.x, r->dim.y, title, NULL, NULL);
	if (icons) glfwSetWindowIcon(r->native_handle, icons_count, icons);
	glfwSetWindowPos(r->native_handle, (vidmode->width / 2) - (r->dim.x / 2), (vidmode->height / 2) - (r->dim.y / 2));
	glfwSetFramebufferSizeCallback(r->native_handle, framebuffer_resize_callback);
	glfwSetWindowUserPointer(r->native_handle, &r->ud);

	return r;
}

ig_window* ig_window_create(const ig_ivec2* dim, const char* title, int full_screen, int monitor, const GLFWimage* icons, int icons_count) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	ig_window* r = (ig_window*) malloc(sizeof(ig_window));
	r->ud.window = r;
	r->resize_requested = false;
	int monitor_count;
	GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
	const GLFWvidmode* vidmode = glfwGetVideoMode(monitors[monitor]);
	r->dim = full_screen ? (ig_ivec2) { .x = vidmode->width, .y = vidmode->height } : *dim;
	r->last_dim.x = r->dim.x;
	r->last_dim.y = r->dim.y;

	r->native_handle = glfwCreateWindow(r->dim.x, r->dim.y, title, full_screen ? monitors[monitor] : NULL, NULL);
	if (icons) glfwSetWindowIcon(r->native_handle, icons_count, icons);
	glfwSetFramebufferSizeCallback(r->native_handle, framebuffer_resize_callback);
	if (!full_screen)
		glfwSetWindowPos(r->native_handle, (vidmode->width / 2) - (r->dim.x / 2), (vidmode->height / 2) - (r->dim.y / 2));
	glfwSetWindowUserPointer(r->native_handle, &r->ud);

	return r;
}

void ig_window_input(ig_window* window) {
	glfwPollEvents();
}

void ig_window_set_fullscreen(ig_window* window, bool fullscreen) {
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	if (fullscreen) {
		window->last_dim.x = window->dim.x;
		window->last_dim.y = window->dim.y;
		glfwSetWindowMonitor(window->native_handle, glfwGetPrimaryMonitor(), 0, 0, vidmode->width, vidmode->height, GLFW_DONT_CARE);
	} else {
		glfwSetWindowMonitor(window->native_handle, NULL, (vidmode->width / 2) - (window->last_dim.x / 2), (vidmode->height / 2) - (window->last_dim.y / 2), window->last_dim.x, window->last_dim.y, GLFW_DONT_CARE);
	}
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
