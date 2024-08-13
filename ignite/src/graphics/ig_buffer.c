#include "ig_buffer.h"
#include "ig_context.h"
#include <stdlib.h>
#include <string.h>

ig_buffer* ig_context_buffer_create(ig_context* context, VkBufferUsageFlags usage, void* data, VkDeviceSize size) {
	ig_buffer* r = malloc(sizeof(ig_buffer));

	VkBuffer staging_buffer;
	VmaAllocation staging_memory;
	VmaAllocationInfo staging_info;

	vmaCreateBuffer(context->allocator, &(VkBufferCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL
	}, &(VmaAllocationCreateInfo) {
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = VK_NULL_HANDLE,
		.pUserData = NULL,
		.priority = 0
	}, &staging_buffer, &staging_memory, &staging_info);

	memcpy(staging_info.pMappedData, data, size);

	vmaCreateBuffer(context->allocator, &(VkBufferCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL
	}, &(VmaAllocationCreateInfo) {
		.flags = 0,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = VK_NULL_HANDLE,
		.pUserData = NULL,
		.priority = 0
	}, &r->buffer, &r->memory, NULL);

	vkResetCommandBuffer(context->transfer_cmd_buffer, 0);
	vkBeginCommandBuffer(context->transfer_cmd_buffer, &(VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	});
	vkCmdCopyBuffer(context->transfer_cmd_buffer, staging_buffer, r->buffer, 1, &(VkBufferCopy) {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = size
	});
	vkEndCommandBuffer(context->transfer_cmd_buffer);
	
	vkResetFences(context->device, 1, &context->transfer_fence);
	vkQueueSubmit(context->queue, 1, &(VkSubmitInfo) {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = NULL,
		.pWaitDstStageMask = NULL,
		.commandBufferCount = 1,
		.pCommandBuffers = &context->transfer_cmd_buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL
	}, context->transfer_fence);

	vkWaitForFences(context->device, 1, &context->transfer_fence, VK_TRUE, UINT64_MAX);
	vmaDestroyBuffer(context->allocator, staging_buffer, staging_memory);

	return r;
}

void ig_context_buffer_destroy(ig_context* context, ig_buffer* buffer) {
	vmaDestroyBuffer(context->allocator, buffer->buffer, buffer->memory);
	free(buffer);
}
