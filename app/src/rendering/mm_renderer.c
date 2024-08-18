#include "mm_renderer.h"
#include <stdlib.h>
#include <string.h>
#include <graphics/ig_buffer.h>
#include <graphics/ig_texture.h>

ig_texture* create_mm_dtexture(mm_renderer* mm_renderer, ig_context* context) {
	ig_texture* r = malloc(context->fif * sizeof(ig_texture));

	for (int i = 0; i < context->fif; i++) {
		VmaAllocationInfo memory_info;

		vmaCreateImage(context->allocator, &(VkImageCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_R8_UNORM,
			.extent = {
				.width = 512,
				.height = 512,
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
		}, &r[i].image, &r[i].memory, NULL);

		vkCreateImageView(context->device, &(VkImageViewCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = r[i].image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8_UNORM,
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
		}, NULL, &r[i].view);

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
				.image = r[i].image,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			});
		vkCmdCopyBufferToImage(context->transfer_cmd_buffer, mm_renderer->transfer_buffer[i].buffer, r[i].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &(VkBufferImageCopy) {
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
			.imageExtent = { .width = 512, .height = 512, .depth = 1 }
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
				.image = r[i].image,
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
	}

	return r;
}

mm_renderer* mm_renderer_create(ig_context* context, unsigned int max_instances) {
	mm_renderer* r = malloc(sizeof(mm_renderer));
	r->instance_count = 0;
	r->instances = malloc(max_instances * sizeof(mm_instance));
	r->curr_sz = 0;
	memset(r->map_data, 0, 512 * 512);

	r->transfer_buffer = ig_context_dbuffer_create(context, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, NULL, 512 * 512);
	for (int i = 0; i < context->fif; i++) {
		memset(r->transfer_buffer[i].data, 255, 512 * 512);
	}

	r->tex = create_mm_dtexture(r, context);

	VkShaderModule vertex_shader = ig_context_create_shader_from_file(context, "app/res/shaders/mmv.spv");
	VkShaderModule fragment_shader = ig_context_create_shader_from_file(context, "app/res/shaders/mmf.spv");

	vkCreateGraphicsPipelines(context->device, VK_NULL_HANDLE, 1, &(VkGraphicsPipelineCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stageCount = 2,
		.pStages = (VkPipelineShaderStageCreateInfo[]) {
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = NULL,
				.flags = 0,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = vertex_shader,
				.pName = "main",
				.pSpecializationInfo = NULL
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = NULL,
				.flags = 0,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = fragment_shader,
				.pName = "main",
				.pSpecializationInfo = NULL
			}
		},
		.pVertexInputState = &(VkPipelineVertexInputStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.vertexBindingDescriptionCount = 2,
			.pVertexBindingDescriptions = (VkVertexInputBindingDescription[]) {
				{
					.binding = 0,
					.stride = 2 * sizeof(float),
					.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				},
				{
					.binding = 1,
					.stride = sizeof(mm_instance),
					.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
				},
			},
			.vertexAttributeDescriptionCount = 4,
			.pVertexAttributeDescriptions = (VkVertexInputAttributeDescription[]) {
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = 0
				},
				{
					.location = 1,
					.binding = 1,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = 0
				},
				{
					.location = 2,
					.binding = 1,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = offsetof(mm_instance, color)
				},
				{
					.location = 3,
					.binding = 1,
					.format = VK_FORMAT_R32_SFLOAT,
					.offset = offsetof(mm_instance, usage)
				}
			}
		},
		.pInputAssemblyState = &(VkPipelineInputAssemblyStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			.primitiveRestartEnable = VK_FALSE
		},
		.pTessellationState = NULL,
		.pViewportState = &(VkPipelineViewportStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &(VkViewport) {
				.x = 0.0f,
				.y = 0.0f,
				.width = (float) (context->default_frame.resolution.x),
				.height = (float) (context->default_frame.resolution.y),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			},
			.scissorCount = 1,
			.pScissors = &(VkRect2D) {
				.offset = { .x = 0, .y = 0 },
				.extent = { 
					.width = context->default_frame.resolution.x,
					.height = context->default_frame.resolution.y
				}
			}
		},
		.pRasterizationState = &(VkPipelineRasterizationStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f
		},
		.pMultisampleState = &(VkPipelineMultisampleStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 0.0f,
			.pSampleMask = NULL,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		},
		.pDepthStencilState = &(VkPipelineDepthStencilStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.depthTestEnable = VK_FALSE,
		},
		.pColorBlendState = &(VkPipelineColorBlendStateCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &(VkPipelineColorBlendAttachmentState) {
				.blendEnable = VK_TRUE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
				.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.colorBlendOp = VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp = VK_BLEND_OP_ADD,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			},
			.blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
		},
		.pDynamicState = NULL,
		.layout = context->standard_layout,
		.renderPass = context->default_frame.render_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	}, NULL, &r->pipeline);

	vkDestroyShaderModule(context->device, fragment_shader, NULL);
	vkDestroyShaderModule(context->device, vertex_shader, NULL);

	r->instance_buffer = ig_context_dbuffer_create(context, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, NULL, max_instances * sizeof(mm_instance));
	r->tex_data_ptr = malloc(context->fif * sizeof(VkDescriptorSet));

	vkAllocateDescriptorSets(context->device, &(VkDescriptorSetAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = context->descriptor_pool,
		.descriptorSetCount = context->fif,
		.pSetLayouts = (VkDescriptorSetLayout[]) { context->texture_layout, context->texture_layout }
	}, r->tex_data_ptr);

	for (int i = 0; i < context->fif; i++) {
		vkUpdateDescriptorSets(context->device, 1, &(VkWriteDescriptorSet) {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->tex_data_ptr[i],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.sampler = context->nearest_sampler,
				.imageView = r->tex[i].view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		}, 0, NULL);
	}

	return r;
}

void mm_renderer_transfer_map(mm_renderer* mm_renderer, ig_context* context, _ig_frame* frame) {
	memcpy(mm_renderer->transfer_buffer[context->frame_idx].data, mm_renderer->map_data, mm_renderer->curr_sz * mm_renderer->curr_sz);
	vkCmdPipelineBarrier(frame->cmd_buffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &(VkImageMemoryBarrier) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_NONE,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = context->queue_family,
			.dstQueueFamilyIndex = context->queue_family,
			.image = mm_renderer->tex[context->frame_idx].image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		});
	vkCmdCopyBufferToImage(frame->cmd_buffer, mm_renderer->transfer_buffer[context->frame_idx].buffer, mm_renderer->tex[context->frame_idx].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &(VkBufferImageCopy) {
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
		.imageExtent = { .width = mm_renderer->curr_sz, .height = mm_renderer->curr_sz, .depth = 1 }
	});
	vkCmdPipelineBarrier(frame->cmd_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &(VkImageMemoryBarrier) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = context->queue_family,
			.dstQueueFamilyIndex = context->queue_family,
			.image = mm_renderer->tex[context->frame_idx].image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		});
}

void mm_renderer_set_map_data(mm_renderer* mm_renderer, const uint8_t* map_data) {
	memcpy(mm_renderer->map_data, map_data, mm_renderer->curr_sz * mm_renderer->curr_sz);
}

void mm_renderer_push(mm_renderer* mm_renderer, const mm_instance* mm_instance, int curr_sz) {
	mm_renderer->instances[mm_renderer->instance_count++] = *mm_instance;
	mm_renderer->curr_sz = curr_sz;
}

void mm_renderer_flush(mm_renderer* mm_renderer, ig_context* context, _ig_frame* frame) {
	memcpy(mm_renderer->instance_buffer[context->frame_idx].data, mm_renderer->instances, mm_renderer->instance_count * sizeof(mm_instance));

	vkCmdBindDescriptorSets(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->standard_layout, 1, 1, &mm_renderer->tex_data_ptr[context->frame_idx], 0, NULL);
	
	vkCmdBindPipeline(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mm_renderer->pipeline);
	vkCmdBindVertexBuffers(frame->cmd_buffer, 1, 1, &mm_renderer->instance_buffer[context->frame_idx].buffer, (VkDeviceSize[]) { 0 });
	vkCmdDraw(frame->cmd_buffer, 4, mm_renderer->instance_count, 0, 0);

	mm_renderer->instance_count = 0;
}

void mm_renderer_destroy(mm_renderer* mm_renderer, ig_context* context) {
	free(mm_renderer->tex_data_ptr);
	for (int i = context->fif - 1; i >= 0; i--) {
		vkDestroyImageView(context->device, mm_renderer->tex[i].view, NULL);
		vmaDestroyImage(context->allocator, mm_renderer->tex[i].image, mm_renderer->tex[i].memory);
	}
	free(mm_renderer->tex);

	ig_context_dbuffer_destroy(context, mm_renderer->transfer_buffer);
	ig_context_dbuffer_destroy(context, mm_renderer->instance_buffer);
	vkDestroyPipeline(context->device, mm_renderer->pipeline, NULL);
	free(mm_renderer->instances);
	free(mm_renderer);
}
