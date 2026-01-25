#include "mm_renderer.h"

#include <string.h>

mm_renderer* mm_renderer_create(tcontext* ctx, VkPipelineLayout layout,
                                VkRenderPass pass) {
  mm_renderer* r = malloc(sizeof(mm_renderer));
  memset(r->minimap, 0, MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE);

  r->minimap_buffer =
      tdbuffer_create(ctx, r->minimap, MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

  r->minimap_tex = malloc(ctx->fif * sizeof(texture*));

  for (int i = 0; i < ctx->fif; i++) {
    r->minimap_tex[i] = create_minimap_texture(ctx, MAX_MINIMAP_SIZE);
  }

  VkShaderModule vertex_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/mmv.spv");
  VkShaderModule fragment_shader =
      tcontext_create_shader(ctx, "app/res/shaders/bin/mmf.spv");

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

void mm_renderer_prerender(mm_renderer* r, tcontext* ctx) {
  tcontext_frame* fr = ctx->frames + ctx->current_frame;

  memcpy(r->minimap_buffer[ctx->current_frame].data, r->minimap, MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE);

  vkCmdPipelineBarrier(
      fr->cmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
          .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->minimap_tex[ctx->current_frame]->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});
  vkCmdCopyBufferToImage(
      fr->cmd, r->minimap_buffer[ctx->current_frame].handle,
      r->minimap_tex[ctx->current_frame]->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
      &(VkBufferImageCopy){0,
                           0,
                           0,
                           {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                           {0, 0, 0},
                           {MAX_MINIMAP_SIZE, MAX_MINIMAP_SIZE, 1}});

  vkCmdPipelineBarrier(
      fr->cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1,
      &(VkImageMemoryBarrier){
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
          .pNext = NULL,
          .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
          .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
          .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
          .srcQueueFamilyIndex = ctx->queue_family,
          .dstQueueFamilyIndex = ctx->queue_family,
          .image = r->minimap_tex[ctx->current_frame]->image,
          .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                               .baseMipLevel = 0,
                               .levelCount = 1,
                               .baseArrayLayer = 0,
                               .layerCount = 1}});
}

void mm_renderer_render(mm_renderer* r, tcontext* ctx) {
  tcontext_frame* fr = ctx->frames + ctx->current_frame;
  vkCmdBindPipeline(fr->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r->pipeline);
  vkCmdDraw(fr->cmd, 4, 1, 0, 0);
}

void mm_renderer_destroy(mm_renderer* r, tcontext* ctx) {
  vkDestroyPipeline(ctx->device, r->pipeline, NULL);
  for (int i = 0; i < ctx->fif; i++) {
    destroy_texture(ctx, r->minimap_tex[i]);
  }
  free(r->minimap_tex);
  tdbuffer_destroy(ctx, r->minimap_buffer);
  free(r);
}
