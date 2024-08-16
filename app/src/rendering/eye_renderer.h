#ifndef EYE_RENDERER_H
#define EYE_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct eye_instance {
	ig_vec4 circ;
	ig_vec2 ratios;
	ig_vec4 color;
} eye_instance;

typedef struct eye_renderer {
	unsigned int instance_count;
	eye_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
} eye_renderer;

eye_renderer* eye_renderer_create(ig_context* context, unsigned int max_instances);
void eye_renderer_push(eye_renderer* eye_renderer, const eye_instance* eye_instance);
void eye_renderer_flush(eye_renderer* eye_renderer, ig_context* context, _ig_frame* frame);
void eye_renderer_destroy(eye_renderer* eye_renderer, ig_context* context);

#endif