#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct sprite_instance {
	ig_vec4 rect;
	ig_vec2 ratios;
	ig_vec4 uv_rect;
	ig_vec3 color;
} sprite_instance;

typedef struct sprite_renderer {
	unsigned int instance_count;
	sprite_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
	VkDescriptorSet sprite_sheet_ptr;
} sprite_renderer;

sprite_renderer* sprite_renderer_create(ig_context* context, const ig_texture* sprite_sheet, unsigned int max_instances);
void sprite_renderer_push(sprite_renderer* sprite_renderer, const sprite_instance* sprite_instance);
void sprite_renderer_flush(sprite_renderer* sprite_renderer, ig_context* context, _ig_frame* frame);
void sprite_renderer_destroy(sprite_renderer* sprite_renderer, ig_context* context);

#endif