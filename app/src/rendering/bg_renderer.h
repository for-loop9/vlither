#ifndef BG_RENDERER_H
#define BG_RENDERER_H

#include <thermite.h>

typedef struct bg_renderer {
  VkPipeline pipeline;
} bg_renderer;

bg_renderer* bg_renderer_create(tcontext* ctx, VkPipelineLayout layout,
                                VkRenderPass pass);
void bg_renderer_render(bg_renderer* r, tcontext* ctx);
void bg_renderer_destroy(bg_renderer* r, tcontext* ctx);

#endif