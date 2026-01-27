#ifndef BP_RENDERER_H
#define BP_RENDERER_H

#include <thermite.h>

typedef struct bp_instance {
  vec4s circle;
  vec4s uv_rect;
  vec4s color;
} bp_instance;

typedef struct bp_renderer {
  VkPipeline pipeline;
  tdbuffer* instance_buffer;
  bp_instance* instances;
  int max_instances;
  int num_instances;
} bp_renderer;

bp_renderer* bp_renderer_create(tcontext* ctx, int max_instances,
                                VkPipelineLayout layout, VkRenderPass pass);
void bp_renderer_push(bp_renderer* r, const bp_instance* instance);
void bp_renderer_render(bp_renderer* r, tcontext* ctx);
void bp_renderer_destroy(bp_renderer* r, tcontext* ctx);

#endif