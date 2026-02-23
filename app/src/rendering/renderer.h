#ifndef RENDERER_H
#define RENDERER_H

#include <stdalign.h>

#include "bg_renderer.h"
#include "fd_renderer.h"
#include "bp_renderer.h"
#include "bst_renderer.h"
#include "bd_renderer.h"
#include "mm_renderer.h"
#include "spr_renderer.h"
#include "texture.h"

typedef struct renderer_image_data {
  VkFramebuffer framebuffer;
  VkImage color;
  VkImageView color_view;
  VkImageView color_view_alpha;
  VmaAllocation memory;
} renderer_image_data;

typedef struct renderer {
  struct {
    alignas(8) vec2 viewport;
    alignas(8) vec2 bg_size;
    alignas(16) vec3 bg_color;
    alignas(16) vec3 bd_color;
    alignas(16) vec3 minimap_circ;
    alignas(8) vec2 view;
    alignas(8) vec2 lview;
    alignas(4) float zoom;
    alignas(4) float bg_opacity;
    alignas(4) float bg_scale;
    alignas(4) float bd_opacity;
    alignas(4) float bd_radius;
    alignas(4) float grd;
    alignas(4) float max_minimap_size;
    alignas(4) float minimap_data_size;
    alignas(4) float minimap_opacity;
  } global;

  texture* tex_atlas;
  texture* bg_tex;

  VkSampler linear_sampler;
  VkSampler nearest_sampler;

  VkRenderPass render_pass;
  VkDescriptorSetLayout global_set_layout;
  VkDescriptorSet* global_set;
  tdbuffer* global_buffer;
  VkPipelineLayout pipeline_layout;
  renderer_image_data* images;
  ivec2 size;
  tbuffer* quad_buffer;

  bg_renderer* bgr;
  fd_renderer* fdr;
  bst_renderer* bsta;
  bp_renderer* bpr;
  bst_renderer* bstb;
  bd_renderer* bdr;
  bp_renderer* astr;
  mm_renderer* mmr;
  spr_renderer* cr;
} renderer;

renderer* renderer_create(tenv* env);
void renderer_render(renderer* r, tcontext* ctx, vec4 clear_color);
void renderer_render_cursor(renderer* r, tcontext* ctx);
void renderer_destroy(renderer* r, tcontext* ctx);
void renderer_resize(renderer* r, tcontext* ctx, ivec2 size);
void renderer_clear_instances(renderer* r);

#endif