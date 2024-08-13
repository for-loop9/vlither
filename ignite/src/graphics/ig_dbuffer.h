#ifndef IG_DBUFFER_H
#define IG_DBUFFER_H

#include "../external/vma/vma.h"

typedef struct ig_context ig_context;

typedef struct ig_dbuffer {
	VkBuffer buffer;
	VmaAllocation memory;
	void* data;
} ig_dbuffer;

ig_dbuffer* ig_context_dbuffer_create(ig_context* context, VkBufferUsageFlags usage, void* data, VkDeviceSize size);
void ig_context_dbuffer_destroy(ig_context* context, ig_dbuffer* buffer);

#endif