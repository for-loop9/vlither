#include <stdio.h>
#include <stdlib.h>

#include "../core/tenv.h"

void twindow_request_refresh(twindow* twindow) {
  twindow->_refresh = true;
}

void window_resize_callback(GLFWwindow* window, int width, int height) {
  tenv* env = glfwGetWindowUserPointer(window);
  env->wnd->size[0] = width;
  env->wnd->size[1] = height;

  if (width > 0 && height > 0) {
    tcontext_resize(env->ctx, env->wnd->size, env->config.vsync);
    env->wnd->_resize_func(env);
    env->wnd->_render_func(env);
  }
}

twindow* twindow_create(tenv* env, trender_func render_func,
                        tresize_func resize_func) {
  twindow* window = malloc(sizeof(twindow));
  window->_render_func = render_func;
  window->_resize_func = resize_func;
  window->_refresh = false;
  window->env = env;
  if (glfwInit() == GLFW_FALSE) {
    printf("Error initializing window\n");
    return NULL;
  }

  glfwWindowHint(GLFW_RESIZABLE,
                 env->config.resizable ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWmonitor* primary = glfwGetPrimaryMonitor();
  const GLFWvidmode* vidmode = glfwGetVideoMode(primary);

  float padding = 0.8f;
  int mw = vidmode->width * padding;
  int mh = mw / env->config.aspect_ratio;
  if (mh > vidmode->height * padding) {
    mh = vidmode->height * padding;
    mw = mh * env->config.aspect_ratio;
  }
  int mx = (vidmode->width - mw) / 2;
  int my = (vidmode->height - mh) / 2;

  if (env->config.fullscreen) {
    window->size[0] = vidmode->width;
    window->size[1] = vidmode->height;
    window->lsize[0] = mw;
    window->lsize[1] = mh;
    window->lpos[0] = mx;
    window->lpos[1] = my;

    window->handle =
        glfwCreateWindow(window->size[0], window->size[1], env->config.title,
                         glfwGetPrimaryMonitor(), NULL);
  } else {
    window->size[0] = mw;
    window->size[1] = mh;
    window->lpos[0] = 0;
    window->lpos[1] = 0;
    window->lsize[0] = vidmode->width;
    window->lsize[1] = vidmode->height;

    glfwWindowHint(GLFW_POSITION_X, mx);
    glfwWindowHint(GLFW_POSITION_Y, my);

    window->handle = glfwCreateWindow(window->size[0], window->size[1],
                                      env->config.title, NULL, NULL);
  }

  if (!window->handle) {
    printf("Error creating window\n");
    return NULL;
  }

  glfwSetWindowUserPointer(window->handle, env);
  glfwSetFramebufferSizeCallback(window->handle, window_resize_callback);

  return window;
}

void twindow_poll_input(twindow* window) {
  glfwPollEvents();
  if (window->_refresh) {
    tcontext_resize(window->env->ctx, window->env->wnd->size, window->env->config.vsync);
    window->_refresh = false;
  }
}
void twindow_wait_input(twindow* window) {
  glfwWaitEvents();
  if (window->_refresh) {
    tcontext_resize(window->env->ctx, window->env->wnd->size, window->env->config.vsync);
    window->_refresh = false;
  }
}

void twindow_toggle_fullscreen(twindow* window) {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);

  if (!window->env->config.fullscreen) {
    glfwGetWindowPos(window->handle, &window->lpos[0], &window->lpos[1]);
    glfwGetWindowSize(window->handle, &window->lsize[0], &window->lsize[1]);

    glfwSetWindowMonitor(window->handle, monitor, 0, 0, mode->width,
                         mode->height, mode->refreshRate);

    window->env->config.fullscreen = true;
  } else {
    glfwRestoreWindow(window->handle);
    glfwSetWindowMonitor(window->handle, NULL, window->lpos[0], window->lpos[1],
                         window->lsize[0], window->lsize[1], mode->refreshRate);
    window->env->config.fullscreen = false;
  }
}

bool twindow_key_down(twindow* window, int key) {
  return glfwGetKey(window->handle, key) == GLFW_PRESS;
}

bool twindow_button_down(twindow* window, int button) {
  return glfwGetMouseButton(window->handle, button) == GLFW_PRESS;
}

bool twindow_closed(twindow* window) {
  return glfwWindowShouldClose(window->handle);
}

void twindow_destroy(twindow* window) {
  glfwTerminate();
  free(window);
}