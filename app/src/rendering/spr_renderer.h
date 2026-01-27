#ifndef SPR_RENDERER_H
#define SPR_RENDERER_H

#include <thermite.h>

typedef struct spr_instance {
  vec4s rect;
  vec4s uv_rect;
  vec4s color;
} spr_instance;

typedef struct spr_renderer {
  VkPipeline pipeline;
  tdbuffer* instance_buffer;
  spr_instance* instances;
  int max_instances;
  int num_instances;
} spr_renderer;

spr_renderer* spr_renderer_create(tcontext* ctx, int max_instances,
                                  VkPipelineLayout layout, VkRenderPass pass);
void spr_renderer_push(spr_renderer* r, const spr_instance* instance);
void spr_renderer_render(spr_renderer* r, tcontext* ctx);
void spr_renderer_destroy(spr_renderer* r, tcontext* ctx);

#endif