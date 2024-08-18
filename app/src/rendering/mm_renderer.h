#ifndef MM_RENDERER_H
#define MM_RENDERER_H

#include <graphics/ig_context.h>
#include <graphics/ig_dbuffer.h>
#include <math/ig_vec2.h>
#include <math/ig_vec3.h>
#include <math/ig_vec4.h>

typedef struct mm_instance {
	ig_vec3 transform;
	ig_vec3 color;
	float usage;
} mm_instance;

typedef struct mm_renderer {
	unsigned int instance_count;
	mm_instance* instances;
	VkPipeline pipeline;
	ig_dbuffer* instance_buffer;
	ig_dbuffer* transfer_buffer;
	VkDescriptorSet* tex_data_ptr;
	ig_texture* tex;
	uint8_t map_data[512 * 512];
	int curr_sz;
} mm_renderer;

mm_renderer* mm_renderer_create(ig_context* context, unsigned int max_instances);
void mm_renderer_transfer_map(mm_renderer* mm_renderer, ig_context* context, _ig_frame* frame);
void mm_renderer_set_map_data(mm_renderer* mm_renderer, const uint8_t* map_data);
void mm_renderer_push(mm_renderer* mm_renderer, const mm_instance* mm_instance, int curr_sz);
void mm_renderer_flush(mm_renderer* mm_renderer, ig_context* context, _ig_frame* frame);
void mm_renderer_destroy(mm_renderer* mm_renderer, ig_context* context);

#endif