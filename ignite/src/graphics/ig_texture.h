#ifndef IG_TEXTURE_H
#define IG_TEXTURE_H

#include "../external/vma/vma.h"
#include "../math/ig_ivec2.h"

typedef struct ig_context ig_context;

typedef struct ig_texture {
	VkImage image;
	VmaAllocation memory;
	VkImageView view;
	ig_ivec2 dim;
} ig_texture;

ig_texture* ig_context_texture_create_from_file(ig_context* context, const char* file_name);
ig_texture* ig_context_texture_create(ig_context* context, void* data, const ig_ivec2* dim);
ig_texture* ig_context_texture_destroy(ig_context* context, ig_texture* image);

#endif