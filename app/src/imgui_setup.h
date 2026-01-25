#ifndef IMGUI_SETUP_H
#define IMGUI_SETUP_H

#include <thermite.h>

void imgui_init(tenv* env);
void imgui_prerender();
void imgui_render(VkCommandBuffer cmd);
void imgui_destroy();

#endif