#ifndef BD_RENDERER_H
#define BD_RENDERER_H

#include <thermite.h>

typedef struct bd_renderer {
  VkPipeline pipeline;
} bd_renderer;

bd_renderer* bd_renderer_create(tcontext* ctx, VkPipelineLayout layout,
                                VkRenderPass pass);
void bd_renderer_render(bd_renderer* r, tcontext* ctx);
void bd_renderer_destroy(bd_renderer* r, tcontext* ctx);

#endif