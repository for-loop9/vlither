#include "circle_renderer.h"
#include <stdlib.h>
#include <string.h>
#include <graphics/ig_buffer.h>

circle_renderer* circle_renderer_create(ig_context* context, unsigned int max_instances) {
	circle_renderer* r = malloc(sizeof(circle_renderer));
	r->instance_count = 0;
	r->instances = malloc(max_instances * sizeof(circle_instance));

	VkShaderModule vertex_shader = ig_context_create_shader_from_file(context, "app/res/shaders/circlev.spv");
	VkShaderModule fragment_shader = ig_context_create_shader_from_file(context, "app/res/shaders/circlef.spv");

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
					.stride = sizeof(circle_instance),
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
					.format = VK_FORMAT_R32G32_SFLOAT,
					.offset = offsetof(circle_instance, ratios)
				},
				{
					.location = 3,
					.binding = 1,
					.format = VK_FORMAT_R32G32B32A32_SFLOAT,
					.offset = offsetof(circle_instance, color)
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
		.pDepthStencilState = NULL,
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

	r->instance_buffer = ig_context_dbuffer_create(context, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, NULL, max_instances * sizeof(circle_instance));

	return r;
}

void circle_renderer_push(circle_renderer* circle_renderer, const circle_instance* circle_instance) {
	circle_renderer->instances[circle_renderer->instance_count++] = *circle_instance;
}

void circle_renderer_flush(circle_renderer* circle_renderer, ig_context* context, _ig_frame* frame) {
	memcpy(circle_renderer->instance_buffer[context->frame_idx].data, circle_renderer->instances, circle_renderer->instance_count * sizeof(circle_instance));
	vkCmdBindPipeline(frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, circle_renderer->pipeline);
	vkCmdBindVertexBuffers(frame->cmd_buffer, 1, 1, &circle_renderer->instance_buffer[context->frame_idx].buffer, (VkDeviceSize[]) { 0 });
	vkCmdDraw(frame->cmd_buffer, 4, circle_renderer->instance_count, 0, 0);
	circle_renderer->instance_count = 0;
}

void circle_renderer_destroy(circle_renderer* circle_renderer, ig_context* context) {
	ig_context_dbuffer_destroy(context, circle_renderer->instance_buffer);
	vkDestroyPipeline(context->device, circle_renderer->pipeline, NULL);
	free(circle_renderer->instances);
	free(circle_renderer);
}
