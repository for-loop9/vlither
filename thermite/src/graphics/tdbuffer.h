#ifndef TDBUFFER_H
#define TDBUFFER_H

#include "tcontext.h"

typedef struct tdbuffer {
  VkBuffer handle;
  VmaAllocation memory;
  void* data;
} tdbuffer;

tdbuffer* tdbuffer_create(tcontext* context, const void* data,
                          VkDeviceSize size, VkBufferUsageFlags usage);
void tdbuffer_destroy(tcontext* context, tdbuffer* buffer);

#endif