#include "ig_dbuffer.h"
#include "ig_context.h"
#include <stdlib.h>
#include <string.h>

ig_dbuffer* ig_context_dbuffer_create(ig_context* context, VkBufferUsageFlags usage, void* data, VkDeviceSize size) {
	ig_dbuffer* r = malloc(context->fif * sizeof(ig_dbuffer));

	for (int i = 0; i < context->fif; i++) {
		VmaAllocationInfo memory_info;

		vmaCreateBuffer(context->allocator, &(VkBufferCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.size = size,
			.usage = usage,
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
		}, &r[i].buffer, &r[i].memory, &memory_info);

		r[i].data = memory_info.pMappedData;
		if (data) memcpy(r[i].data, data, size);
	}

	return r;
}

void ig_context_dbuffer_destroy(ig_context* context, ig_dbuffer* buffer) {
	for (int i = context->fif - 1; i >= 0; i--) {
		vmaDestroyBuffer(context->allocator, buffer[i].buffer, buffer[i].memory);
	}
	free(buffer);
}
