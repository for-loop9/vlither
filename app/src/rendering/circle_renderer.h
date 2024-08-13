#ifndef CIRCLE_RENDERER_H
#define CIRCLE_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct circle_instance {
	ig_vec3 circ;
	ig_vec2 ratios;
	ig_vec4 color;
} circle_instance;

typedef struct circle_renderer {
	unsigned int instance_count;
	circle_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
} circle_renderer;

circle_renderer* circle_renderer_create(ig_context* context, unsigned int max_instances);
void circle_renderer_push(circle_renderer* circle_renderer, const circle_instance* circle_instance);
void circle_renderer_flush(circle_renderer* circle_renderer, ig_context* context, _ig_frame* frame);
void circle_renderer_destroy(circle_renderer* circle_renderer, ig_context* context);

#endif