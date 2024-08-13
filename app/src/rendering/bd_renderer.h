#ifndef BD_RENDERER_H
#define BD_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct bd_instance {
	ig_vec3 circ;
	ig_vec3 color;
} bd_instance;

typedef struct bd_renderer {
	unsigned int instance_count;
	bd_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
} bd_renderer;

bd_renderer* ig_bd_renderer_create(ig_context* context, unsigned int max_instances);
void ig_bd_renderer_push(bd_renderer* bd_renderer, const bd_instance* bd_instance);
void ig_bd_renderer_flush(bd_renderer* bd_renderer, ig_context* context, _ig_frame* frame);
void ig_bd_renderer_destroy(bd_renderer* bd_renderer, ig_context* context);

#endif