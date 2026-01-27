#include "texture.h"
#include <external/stb/stb_image.h>
#include <string.h>

texture* create_mipmap_texture(tcontext* ctx, const char* filename) {
  texture* r = malloc(sizeof(texture));

  VkBuffer staging_buffer;
  VmaAllocation staging_memory;
  VmaAllocationInfo staging_info;

  int w, h, c;
  stbi_uc* data = stbi_load(filename, &w, &h, &c, 4);
  int mip_levels = (uint32_t)(floorf(log2f(GLM_MAX(w, h))) + 1);

  vmaCreateBuffer(
      ctx->allocator,
      &(VkBufferCreateInfo){.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                            .pNext = NULL,
                            .flags = 0,
                            .size = w * h * 4,
                            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE},
      &(VmaAllocationCreateInfo){
          .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                   VMA_ALLOCATION_CREATE_MAPPED_BIT,
          .usage = VMA_MEMORY_USAGE_AUTO},
      &staging_buffer, &staging_memory, &staging_info);

  memcpy(staging_info.pMappedData, data, w * h * 4);
  stbi_image_free(data);

  vmaCreateImage(
      ctx->allocator,
      &(VkImageCreateInfo){.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .imageType = VK_IMAGE_TYPE_2D,
                           .format = VK_FORMAT_R8G8B8A8_UNORM,
                           .extent = {w, h, 1},
                           .mipLevels = mip_levels,
                           .arrayLayers = 1,
                           .samples = VK_SAMPLE_COUNT_1_BIT,
                           .tiling = VK_IMAGE_TILING_OPTIMAL,
                           .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT,
                           .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                           .queueFamilyIndexCount = 0,
                           .pQueueFamilyIndices = NULL,
                           .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED},
      &(VmaAllocationCreateInfo){
          .flags = 0, .usage = VMA_MEMORY_USAGE_AUTO, .priority = 1.0f},
      &r->image, &r->memory, NULL);

  vkResetCommandBuffer(ctx->transfer_cmd, 0);
  vkBeginCommandBuffer(ctx->transfer_cmd,
                       &(VkCommandBufferBeginInfo){
                           .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .pInheritanceInfo = NULL});
  vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_NONE,
          .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});
  vkCmdCopyBufferToImage(
      ctx->transfer_cmd, staging_buffer, r->image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &(VkBufferImageCopy){
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .mipLevel = 0,
                             .baseArrayLayer = 0,
                             .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {w, h, 1}});

  vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});

  for (int i = 1; i < mip_levels; i++) {
    vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_NONE,
          .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = i,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});

    vkCmdBlitImage(ctx->transfer_cmd, r->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, r->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &(VkImageBlit){
      .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, 1},
      .srcOffsets = {{0, 0, 0}, {(int32_t)(w >> (i - 1)), (int32_t)(h >> (i - 1)), 1}},
      .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1},
      .dstOffsets = {{0, 0, 0}, {(int32_t)(w >> i), (int32_t)(h >> i), 1}}
    }, VK_FILTER_LINEAR);

    vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = i,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});
  }

  vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
          .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = mip_levels,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});
                               
  vkEndCommandBuffer(ctx->transfer_cmd);

  vkQueueSubmit(ctx->queue, 1,
                &(VkSubmitInfo){.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = NULL,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &ctx->transfer_cmd},
                ctx->transfer_fence);

  vkWaitForFences(ctx->device, 1, &ctx->transfer_fence, VK_TRUE, UINT64_MAX);
  vkResetFences(ctx->device, 1, &ctx->transfer_fence);

  vkCreateImageView(
      ctx->device,
      &(VkImageViewCreateInfo){
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .image = r->image,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = VK_FORMAT_R8G8B8A8_UNORM,
          .components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY},
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = mip_levels,
                               .baseArrayLayer = 0,
                               .layerCount = 1}},
      NULL, &r->view);

  vmaDestroyBuffer(ctx->allocator, staging_buffer, staging_memory);

  r->size[0] = w;
  r->size[1] = h;

  return r;
}

texture* create_minimap_texture(tcontext* ctx, int width) {
  texture* r = malloc(sizeof(texture));
  vmaCreateImage(
      ctx->allocator,
      &(VkImageCreateInfo){
          .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .imageType = VK_IMAGE_TYPE_2D,
          .format = VK_FORMAT_R8_UNORM,
          .extent = {width, width, 1},
          .mipLevels = 1,
          .arrayLayers = 1,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
          .queueFamilyIndexCount = 0,
          .pQueueFamilyIndices = NULL,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED},
      &(VmaAllocationCreateInfo){
          .flags = 0, .usage = VMA_MEMORY_USAGE_AUTO, .priority = 1.0f},
      &r->image, &r->memory, NULL);

  vkResetCommandBuffer(ctx->transfer_cmd, 0);
  vkBeginCommandBuffer(ctx->transfer_cmd,
                       &(VkCommandBufferBeginInfo){
                           .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .pInheritanceInfo = NULL});
  vkCmdPipelineBarrier(
      ctx->transfer_cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_NONE,
          .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});

  vkEndCommandBuffer(ctx->transfer_cmd);

  vkQueueSubmit(ctx->queue, 1,
                &(VkSubmitInfo){.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = NULL,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &ctx->transfer_cmd},
                ctx->transfer_fence);

  vkWaitForFences(ctx->device, 1, &ctx->transfer_fence, VK_TRUE, UINT64_MAX);
  vkResetFences(ctx->device, 1, &ctx->transfer_fence);

  vkCreateImageView(
      ctx->device,
      &(VkImageViewCreateInfo){
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .image = r->image,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = VK_FORMAT_R8_UNORM,
          .components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY,
                         VK_COMPONENT_SWIZZLE_IDENTITY},
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}},
      NULL, &r->view);

  r->size[0] = width;
  r->size[1] = width;

  return r;
}

void destroy_texture(tcontext* ctx, texture* tex) {
  vkDestroyImageView(ctx->device, tex->view, NULL);
  vmaDestroyImage(ctx->allocator, tex->image, tex->memory);

  free(tex);
}
