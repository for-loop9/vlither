#ifndef bst_RENDERER_H
#define bst_RENDERER_H

#include <thermite.h>

typedef struct bst_instance {
  vec3s circle;
  vec4s color;
} bst_instance;

typedef struct bst_renderer {
  VkPipeline pipeline;
  tdbuffer* instance_buffer;
  bst_instance* instances;
  int max_instances;
  int num_instances;
} bst_renderer;

bst_renderer* bst_renderer_create(tcontext* ctx, int max_instances,
                                VkPipelineLayout layout, VkRenderPass pass);
void bst_renderer_push(bst_renderer* r, const bst_instance* instance);
void bst_renderer_render(bst_renderer* r, tcontext* ctx);
void bst_renderer_destroy(bst_renderer* r, tcontext* ctx);

#endif