#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct ig_text_instance {
	ig_vec3 transform;
	ig_vec4 uv_rect;
	ig_vec4 color;
} ig_text_instance;

typedef struct text_renderer {
	unsigned int instance_count;
	ig_text_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
	VkDescriptorSet font_sheet_ptr;
} text_renderer;

text_renderer* text_renderer_create(ig_context* context, const ig_texture* text_sheet, unsigned int max_instances);
void text_renderer_push(text_renderer* text_renderer, const char* str, const ig_vec3* transform, const ig_vec4* color, ig_vec3* transform_out);
void text_renderer_flush(text_renderer* text_renderer, ig_context* context, _ig_frame* frame);
void text_renderer_destroy(text_renderer* text_renderer, ig_context* context);

#endif