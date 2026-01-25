#include "tdbuffer.h"

#include <string.h>

tdbuffer* tdbuffer_create(tcontext* context, const void* data,
                          VkDeviceSize size, VkBufferUsageFlags usage) {
  tdbuffer* buffer = malloc(context->fif * sizeof(tdbuffer));

  for (int i = 0; i < context->fif; i++) {
    VmaAllocationInfo alloc_info;

    vmaCreateBuffer(
        context->allocator,
        &(VkBufferCreateInfo){.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                              .pNext = NULL,
                              .flags = 0,
                              .size = size,
                              .usage = usage,
                              .sharingMode = VK_SHARING_MODE_EXCLUSIVE},
        &(VmaAllocationCreateInfo){
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO},
        &buffer[i].handle, &buffer[i].memory, &alloc_info);

    buffer[i].data = alloc_info.pMappedData;

    if (data) memcpy(buffer[i].data, data, size);
  }

  return buffer;
}

void tdbuffer_destroy(tcontext* context, tdbuffer* buffer) {
  for (int i = 0; i < context->fif; i++)
    vmaDestroyBuffer(context->allocator, buffer[i].handle, buffer[i].memory);
  free(buffer);
}