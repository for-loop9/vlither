#ifndef MM_RENDERER_H
#define MM_RENDERER_H

#include <thermite.h>
#include "texture.h"
#include "../constants.h"

typedef struct mm_renderer {
  VkPipeline pipeline;
  texture** minimap_tex;
  tdbuffer* minimap_buffer;

  uint8_t minimap[MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE];
} mm_renderer;

mm_renderer* mm_renderer_create(tcontext* ctx, VkPipelineLayout layout,
                                VkRenderPass pass);

void mm_renderer_prerender(mm_renderer* r, tcontext* ctx);
void mm_renderer_render(mm_renderer* r, tcontext* ctx);
void mm_renderer_destroy(mm_renderer* r, tcontext* ctx);

#endif