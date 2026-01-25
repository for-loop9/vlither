#include "../core/tenv.h"

#include "../util/tdarray.h"
#include "twindow.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  tenv* env = glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS)
    tdarray_push(&env->kb->keys_pressed, &key);
  else if (action == GLFW_RELEASE)
    tdarray_push(&env->kb->keys_released, &key);
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
  tenv* env = glfwGetWindowUserPointer(window);
  env->kb->char_pressed = (char)codepoint;
}

tkeyboard* tkeyboard_create(twindow* window) {
  glfwSetKeyCallback(window->handle, key_callback);
  glfwSetCharCallback(window->handle, char_callback);

  tkeyboard* r = malloc(sizeof(tkeyboard));
  r->keys_pressed = tdarray_create(int);
  r->keys_released = tdarray_create(int);

  return r;
}

void tkeyboard_update(tkeyboard* keyboard) {
  tdarray_clear(keyboard->keys_pressed);
  tdarray_clear(keyboard->keys_released);
  keyboard->char_pressed = 0;
}

int tkeyboard_key_pressed(tkeyboard* keyboard, int key) {
  return tdarray_find(keyboard->keys_pressed, &key) != -1;
}

int tkeyboard_key_released(tkeyboard* keyboard, int key) {
  return tdarray_find(keyboard->keys_released, &key) != -1;
}

void tkeyboard_destroy(tkeyboard* keyboard) {
  tdarray_destroy(keyboard->keys_released);
  tdarray_destroy(keyboard->keys_pressed);
  free(keyboard);
}