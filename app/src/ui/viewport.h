#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <thermite.h>

void ui_viewport_init(tenv* env);
void ui_viewport(tenv* env);
void ui_viewport_resize(tenv* env);
void ui_viewport_destroy(tenv* env);

#endif