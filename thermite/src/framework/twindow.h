#ifndef TWINDOW_H
#define TWINDOW_H

#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include <stdbool.h>

typedef struct tkeyboard tkeyboard;
typedef struct tmouse tmouse;
typedef struct twindow twindow;
typedef struct tenv tenv;

typedef void (*trender_func)(tenv* env);
typedef void (*tresize_func)(tenv* env);

typedef struct twindow {
  GLFWwindow* handle;
  ivec2 size;
  ivec2 lsize;
  ivec2 lpos;
  trender_func _render_func;
  tresize_func _resize_func;
  tenv* env;
  bool _refresh;
} twindow;

void twindow_request_refresh(twindow* twindow);
twindow* twindow_create(tenv* env, trender_func render_func,
                        tresize_func resize_func);
void twindow_poll_input(twindow* window);
void twindow_wait_input(twindow* window);
void twindow_toggle_fullscreen(twindow* window);
bool twindow_key_down(twindow* window, int key);
bool twindow_button_down(twindow* window, int button);
bool twindow_closed(twindow* window);
void twindow_destroy(twindow* window);

#endif