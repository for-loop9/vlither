#include "tbuffer.h"

#include <string.h>

tbuffer* tbuffer_create(tcontext* context, const void* data, VkDeviceSize size,
                        VkBufferUsageFlags usage) {
  tbuffer* buffer = malloc(sizeof(tbuffer));

  vmaCreateBuffer(
      context->allocator,
      &(VkBufferCreateInfo){.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0,
                            .size = size,
                            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE},
      &(VmaAllocationCreateInfo){.flags = 0, .usage = VMA_MEMORY_USAGE_AUTO},
      &buffer->handle, &buffer->memory, NULL);

  VkBuffer staging_buffer;
  VmaAllocation staging_memory;
  VmaAllocationInfo staging_info;

  vmaCreateBuffer(
      context->allocator,
      &(VkBufferCreateInfo){.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0,
                            .size = size,
                            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE},
      &(VmaAllocationCreateInfo){
          .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                   VMA_ALLOCATION_CREATE_MAPPED_BIT,
          .usage = VMA_MEMORY_USAGE_AUTO},
      &staging_buffer, &staging_memory, &staging_info);

  memcpy(staging_info.pMappedData, data, size);

  vkResetCommandBuffer(context->transfer_cmd, 0);
  vkBeginCommandBuffer(context->transfer_cmd,
                       &(VkCommandBufferBeginInfo){
                           .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .pInheritanceInfo = NULL});
  vkCmdCopyBuffer(
      context->transfer_cmd, staging_buffer, buffer->handle, 1,
      &(VkBufferCopy){.srcOffset = 0, .dstOffset = 0, .size = size});
  vkEndCommandBuffer(context->transfer_cmd);

  vkQueueSubmit(context->queue, 1,
                &(VkSubmitInfo){.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = NULL,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &context->transfer_cmd},
                context->transfer_fence);

  vkWaitForFences(context->device, 1, &context->transfer_fence, VK_TRUE,
                  UINT64_MAX);
  vkResetFences(context->device, 1, &context->transfer_fence);

  vmaDestroyBuffer(context->allocator, staging_buffer, staging_memory);
  return buffer;
}

void tbuffer_destroy(tcontext* context, tbuffer* buffer) {
  vmaDestroyBuffer(context->allocator, buffer->handle, buffer->memory);
  free(buffer);
}