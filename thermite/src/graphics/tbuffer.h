#ifndef TBUFFER_H
#define TBUFFER_H

#include "tcontext.h"

typedef struct tbuffer {
  VkBuffer handle;
  VmaAllocation memory;
} tbuffer;

tbuffer* tbuffer_create(tcontext* context, const void* data, VkDeviceSize size,
                        VkBufferUsageFlags usage);
void tbuffer_destroy(tcontext* context, tbuffer* buffer);

#endif