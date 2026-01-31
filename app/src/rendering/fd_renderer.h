#ifndef FD_RENDERER_H
#define FD_RENDERER_H

#include <thermite.h>

typedef struct fd_instance {
  vec3s circle;
  vec4s color;
  float flicker;
} fd_instance;

typedef struct fd_renderer {
  VkPipeline pipelines[2];
  tdbuffer* instance_buffer;
  tdbuffer* p_instance_buffer;
  fd_instance* instances;
  fd_instance* p_instances;
  int max_instances;
  int num_instances;
  int num_p_instances;
  int max_p_instances;
  int pipeline_idx;
} fd_renderer;

fd_renderer* fd_renderer_create(tcontext* ctx, int max_instance, int max_p_instances,
                                VkPipelineLayout layout, VkRenderPass pass);
void fd_renderer_push(fd_renderer* r, const fd_instance* instance);
void fd_renderer_push_p(fd_renderer* r, const fd_instance* instance);
void fd_renderer_render(fd_renderer* r, tcontext* ctx);
void fd_renderer_destroy(fd_renderer* r, tcontext* ctx);

#endif