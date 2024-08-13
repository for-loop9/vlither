#ifndef IG_BUFFER_H
#define IG_BUFFER_H

#include "../external/vma/vma.h"

typedef struct ig_context ig_context;

typedef struct ig_buffer {
	VkBuffer buffer;
	VmaAllocation memory;
} ig_buffer;

ig_buffer* ig_context_buffer_create(ig_context* context, VkBufferUsageFlags usage, void* data, VkDeviceSize size);
void ig_context_buffer_destroy(ig_context* context, ig_buffer* buffer);

#endif