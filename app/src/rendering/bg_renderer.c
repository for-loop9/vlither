#include "bg_renderer.h"

#include <string.h>

bg_renderer* bg_renderer_create(tcontext* ctx, VkPipelineLayout layout,
                                VkRenderPass pass) {
  bg_renderer* r = malloc(sizeof(bg_renderer));

  VkShaderModule vertex_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/bgv.spv");
  VkShaderModule fragment_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/bgf.spv");

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
                  .vertexBindingDescriptionCount = 1,
                  .pVertexBindingDescriptions =
                      (VkVertexInputBindingDescription[]){
                          {.binding = 0,
                           .stride = 2 * sizeof(float),
                           .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}},
                  .vertexAttributeDescriptionCount = 1,
                  .pVertexAttributeDescriptions =
                      (VkVertexInputAttributeDescription[]){
                          {.location = 0,
                           .binding = 0,
                           .format = VK_FORMAT_R32G32_SFLOAT,
                           .offset = 0}}},
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
                          .dstAlphaBlendFactor =
                              VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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

  return r;
}

void bg_renderer_render(bg_renderer* r, tcontext* ctx) {
  tcontext_frame* fr = ctx->frames + ctx->current_frame;
  vkCmdBindPipeline(fr->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r->pipeline);
  vkCmdDraw(fr->cmd, 4, 1, 0, 0);
}

void bg_renderer_destroy(bg_renderer* r, tcontext* ctx) {
  vkDestroyPipeline(ctx->device, r->pipeline, NULL);
  free(r);
}
