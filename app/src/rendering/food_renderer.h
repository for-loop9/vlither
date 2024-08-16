#ifndef FOOD_RENDERER_H
#define FOOD_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct food_instance {
	ig_vec3 circ;
	ig_vec2 ratios;
	ig_vec4 color;
} food_instance;

typedef struct food_renderer {
	unsigned int instance_count;
	food_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
} food_renderer;

food_renderer* food_renderer_create(ig_context* context, unsigned int max_instances);
void food_renderer_push(food_renderer* food_renderer, const food_instance* food_instance);
void food_renderer_flush(food_renderer* food_renderer, ig_context* context, _ig_frame* frame);
void food_renderer_destroy(food_renderer* food_renderer, ig_context* context);

#endif