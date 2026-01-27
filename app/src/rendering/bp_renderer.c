#include "bp_renderer.h"
#include <string.h>

bp_renderer* bp_renderer_create(tcontext* ctx, int max_instances,
                                VkPipelineLayout layout, VkRenderPass pass) {
  bp_renderer* r = malloc(sizeof(bp_renderer));

  VkShaderModule vertex_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/bpv.spv");
  VkShaderModule fragment_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/bpf.spv");

  vkCreateGraphicsPipelines(
      ctx->device, NULL, 1,
      &(VkGraphicsPipelineCreateInfo){
          .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .stageCount = 2,
          .pStages =
              (VkPipelineShaderStageCreateInfo[]){
                  {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                   .pNext = NULL,
                   .flags = 0,
                   .stage = VK_SHADER_STAGE_VERTEX_BIT,
                   .module = vertex_shader,
                   .pName = "main",
                   .pSpecializationInfo = NULL},
                  {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                   .pNext = NULL,
                   .flags = 0,
                   .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                   .module = fragment_shader,
                   .pName = "main",
                   .pSpecializationInfo = NULL}},
          .pVertexInputState =
              &(VkPipelineVertexInputStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .vertexBindingDescriptionCount = 2,
                  .pVertexBindingDescriptions =
                      (VkVertexInputBindingDescription[]){
                          {.binding = 0,
                           .stride = 2 * sizeof(float),
                           .inputRate = VK_VERTEX_INPUT_RATE_VERTEX},
                          {.binding = 1,
                           .stride = sizeof(bp_instance),
                           .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE},
                      },
                  .vertexAttributeDescriptionCount = 4,
                  .pVertexAttributeDescriptions =
                      (VkVertexInputAttributeDescription[]){
                          {.location = 0,
                           .binding = 0,
                           .format = VK_FORMAT_R32G32_SFLOAT,
                           .offset = 0},
                          {.location = 1,
                           .binding = 1,
                           .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                           .offset = 0},
                          {.location = 2,
                           .binding = 1,
                           .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                           .offset = offsetof(bp_instance, uv_rect)},
                          {.location = 3,
                           .binding = 1,
                           .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                           .offset = offsetof(bp_instance, color)}}},
          .pInputAssemblyState =
              &(VkPipelineInputAssemblyStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                  .primitiveRestartEnable = VK_FALSE},
          .pTessellationState = NULL,
          .pViewportState =
              &(VkPipelineViewportStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .viewportCount = 1,
                  .scissorCount = 1},
          .pRasterizationState =
              &(VkPipelineRasterizationStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
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
                  .lineWidth = 1.0f},
          .pMultisampleState =
              &(VkPipelineMultisampleStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                  .sampleShadingEnable = VK_FALSE,
                  .minSampleShading = 0.0f,
                  .pSampleMask = NULL,
                  .alphaToCoverageEnable = VK_FALSE,
                  .alphaToOneEnable = VK_FALSE},
          .pDepthStencilState =
              &(VkPipelineDepthStencilStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .depthTestEnable = VK_FALSE,
              },
          .pColorBlendState =
              &(VkPipelineColorBlendStateCreateInfo){
                  .sType =
                      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .logicOpEnable = VK_FALSE,
                  .logicOp = VK_LOGIC_OP_COPY,
                  .attachmentCount = 1,
                  .pAttachments =
                      &(VkPipelineColorBlendAttachmentState){
                          .blendEnable = VK_TRUE,
                          .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                          .dstColorBlendFactor =
                              VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                          .colorBlendOp = VK_BLEND_OP_ADD,
                          .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                          .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                          .alphaBlendOp = VK_BLEND_OP_ADD,
                          .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                            VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT |
                                            VK_COLOR_COMPONENT_A_BIT},
                  .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}},
          .pDynamicState =
              &(VkPipelineDynamicStateCreateInfo){
                  .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .dynamicStateCount = 2,
                  .pDynamicStates =
                      (VkDynamicState[]){VK_DYNAMIC_STATE_VIEWPORT,
                                         VK_DYNAMIC_STATE_SCISSOR}},
          .layout = layout,
          .renderPass = pass,
          .subpass = 0,
          .basePipelineHandle = VK_NULL_HANDLE,
          .basePipelineIndex = 0},
      NULL, &r->pipeline);

  vkDestroyShaderModule(ctx->device, fragment_shader, NULL);
	vkDestroyShaderModule(ctx->device, vertex_shader, NULL);

	r->instance_buffer = tdbuffer_create(ctx, NULL, max_instances * sizeof(bp_instance), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  r->instances = malloc(max_instances * sizeof(bp_instance));
  r->max_instances = max_instances;
  r->num_instances = 0;

  return r;
}

void bp_renderer_push(bp_renderer* r, const bp_instance* instance) {
  if (r->num_instances >= r->max_instances) {
    return;
  }
  r->instances[r->num_instances++] = *instance;
}

void bp_renderer_render(bp_renderer* r, tcontext* ctx) {
  tcontext_frame* fr = ctx->frames + ctx->current_frame;
  memcpy(r->instance_buffer[ctx->current_frame].data, r->instances, r->num_instances * sizeof(bp_instance));
  vkCmdBindPipeline(fr->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r->pipeline);
  vkCmdBindVertexBuffers(fr->cmd, 1, 1, &r->instance_buffer[ctx->current_frame].handle, (VkDeviceSize[]){0});
  vkCmdDraw(fr->cmd, 4, r->num_instances, 0, 0);
}

void bp_renderer_destroy(bp_renderer* r, tcontext* ctx) {
  free(r->instances);
  tdbuffer_destroy(ctx, r->instance_buffer);
  vkDestroyPipeline(ctx->device, r->pipeline, NULL);
  free(r);
}
