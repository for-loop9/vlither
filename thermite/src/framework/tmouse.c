#include "../core/tenv.h"

#include "../util/tdarray.h"
#include "twindow.h"

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  tenv* env = glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS)
    tdarray_push(&env->ms->buttons_pressed, &button);
  else if (action == GLFW_RELEASE)
    tdarray_push(&env->ms->buttons_released, &button);
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
  tenv* env = glfwGetWindowUserPointer(window);
  env->ms->pos[0] = x;
  env->ms->pos[1] = y;
}

void mouse_wheel_callback(GLFWwindow* window, double x, double y) {
  tenv* env = glfwGetWindowUserPointer(window);
  env->ms->dwheel = (float)y;
}

tmouse* tmouse_create(twindow* window) {
  glfwSetMouseButtonCallback(window->handle, mouse_button_callback);
  glfwSetCursorPosCallback(window->handle, mouse_pos_callback);
  glfwSetScrollCallback(window->handle, mouse_wheel_callback);

  tmouse* r = malloc(sizeof(tmouse));
  r->buttons_pressed = tdarray_create(int);
  r->buttons_released = tdarray_create(int);
  glm_vec2_zero(r->pos);
  glm_vec2_zero(r->prev_pos);
  glm_vec2_zero(r->delta);
  r->dwheel = 0;

  return r;
}

void tmouse_update(tmouse* mouse) {
  tdarray_clear(mouse->buttons_pressed);
  tdarray_clear(mouse->buttons_released);

  glm_vec2_sub(mouse->pos, mouse->prev_pos, mouse->delta);
  glm_vec2_copy(mouse->pos, mouse->prev_pos);
  mouse->dwheel = 0;
}

int tmouse_button_pressed(tmouse* mouse, int button) {
  return tdarray_find(mouse->buttons_pressed, &button) != -1;
}

int tmouse_button_released(tmouse* mouse, int button) {
  return tdarray_find(mouse->buttons_released, &button) != -1;
}

void tmouse_destroy(tmouse* mouse) {
  tdarray_destroy(mouse->buttons_released);
  tdarray_destroy(mouse->buttons_pressed);
  free(mouse);
}