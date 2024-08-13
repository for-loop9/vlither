#include "ig_texture.h"
#include "ig_context.h"
#include <string.h>
#include <stdlib.h>
#include "../external/stb/stb_image.h"

ig_texture* ig_context_texture_create_from_file(ig_context* context, const char* file_name) {
	ig_ivec2 img_dim;
	int c;
	stbi_uc* img_data = stbi_load(file_name, &img_dim.x, &img_dim.y, &c, 4);
	ig_texture* r = ig_context_texture_create(context, img_data, &img_dim);
	stbi_image_free(img_data);
	return r;
}

ig_texture* ig_context_texture_create(ig_context* context, void* data, const ig_ivec2* dim) {
	ig_texture* r = malloc(sizeof(ig_texture));
	r->dim = *dim;

	VkBuffer staging_buffer;
	VmaAllocation staging_memory;
	VmaAllocationInfo staging_info;

	vmaCreateBuffer(context->allocator, &(VkBufferCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = dim->x * dim->y * 4,
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

	memcpy(staging_info.pMappedData, data, dim->x * dim->y * 4);

	vmaCreateImage(context->allocator, &(VkImageCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_UNORM,
		.extent = {
			.width = dim->x,
			.height = dim->y,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	}, &(VmaAllocationCreateInfo) {
		.flags = 0,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = VK_NULL_HANDLE,
		.pUserData = NULL,
		.priority = 0
	}, &r->image, &r->memory, NULL);

	vkResetCommandBuffer(context->transfer_cmd_buffer, 0);
	vkBeginCommandBuffer(context->transfer_cmd_buffer, &(VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	});
	vkCmdPipelineBarrier(context->transfer_cmd_buffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &(VkImageMemoryBarrier) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_NONE,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = context->queue_family,
			.dstQueueFamilyIndex = context->queue_family,
			.image = r->image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		});
	vkCmdCopyBufferToImage(context->transfer_cmd_buffer, staging_buffer, r->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &(VkBufferImageCopy) {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageOffset = { .x = 0, .y = 0, .z = 0 },
		.imageExtent = { .width = dim->x, .height = dim->y, .depth = 1 }
	});
	vkCmdPipelineBarrier(context->transfer_cmd_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &(VkImageMemoryBarrier) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = context->queue_family,
			.dstQueueFamilyIndex = context->queue_family,
			.image = r->image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
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

	vkCreateImageView(context->device, &(VkImageViewCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = r->image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_UNORM,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	}, NULL, &r->view);

	return r;
}

ig_texture* ig_context_texture_destroy(ig_context* context, ig_texture* image) {
	vkDestroyImageView(context->device, image->view, NULL);
	vmaDestroyImage(context->allocator, image->image, image->memory);
	free(image);
}
