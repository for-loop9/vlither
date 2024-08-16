#ifndef BP_RENDERER_H
#define BP_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct bp_instance {
	ig_vec4 circ;
	ig_vec2 ratios;
	ig_vec4 color;
} bp_instance;

typedef struct bp_renderer {
	unsigned int instance_count;
	bp_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
} bp_renderer;

bp_renderer* bp_renderer_create(ig_context* context, unsigned int max_instances);
void bp_renderer_push(bp_renderer* bp_renderer, const bp_instance* bp_instance);
void bp_renderer_flush(bp_renderer* bp_renderer, ig_context* context, _ig_frame* frame);
void bp_renderer_destroy(bp_renderer* bp_renderer, ig_context* context);

#endif