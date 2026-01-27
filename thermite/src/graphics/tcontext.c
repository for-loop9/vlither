#include "tcontext.h"

#include <string.h>

void _tcontext_create_instance(tcontext* context) {
  uint32_t instance_ext_count;
  const char** instance_ext_names =
      glfwGetRequiredInstanceExtensions(&instance_ext_count);

  vkCreateInstance(
      &(VkInstanceCreateInfo){
          .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .pApplicationInfo =
              &(VkApplicationInfo){.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                   .pNext = NULL,
                                   .pApplicationName = "app",
                                   .applicationVersion = 1,
                                   .pEngineName = "thermite",
                                   .engineVersion = 1,
                                   .apiVersion = VK_API_VERSION_1_0},
#ifdef TDEBUG
          .enabledLayerCount = 1,
#else
          .enabledLayerCount = 0,
#endif
          .ppEnabledLayerNames = (const char*[]){"VK_LAYER_KHRONOS_validation"},
          .enabledExtensionCount = instance_ext_count,
          .ppEnabledExtensionNames = instance_ext_names},
      NULL, &context->instance);
}

void _tcontext_create_surface(tcontext* context, twindow* window) {
  glfwCreateWindowSurface(context->instance, window->handle, NULL,
                          &context->surface);
}

int _tcontext_select_device(tcontext* context) {
  uint32_t device_count;
  vkEnumeratePhysicalDevices(context->instance, &device_count, NULL);
  VkPhysicalDevice* devices = malloc(device_count * sizeof(VkPhysicalDevice));
  typedef struct {
    int score;
    VkSurfaceFormatKHR selected_format;
    int selected_queue;
    bool supports_immediate;
    bool supports_fifo;
    bool supports_swapchain;
  } score;
  score* scores = malloc(device_count * sizeof(score));
  vkEnumeratePhysicalDevices(context->instance, &device_count, devices);

  for (int i = 0; i < device_count; i++) {
    scores[i].score = -1;
    int selected_format = -1;
    scores[i].selected_queue = -1;
    scores[i].supports_immediate = false;
    scores[i].supports_fifo = false;

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(devices[i], &properties);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], context->surface,
                                         &format_count, NULL);
    VkSurfaceFormatKHR* formats =
        malloc(format_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], context->surface,
                                         &format_count, formats);

    for (int j = 0; j < format_count; j++) {
      if ((formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
           formats[j].format == VK_FORMAT_R8G8B8A8_UNORM) ||
          (formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
           formats[j].format == VK_FORMAT_B8G8R8A8_UNORM)) {
        selected_format = j;
        break;
      }
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], context->surface,
                                              &present_mode_count, NULL);
    VkPresentModeKHR* present_modes =
        malloc(present_mode_count * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        devices[i], context->surface, &present_mode_count, present_modes);

    for (int j = 0; j < present_mode_count; j++) {
      if (present_modes[j] == VK_PRESENT_MODE_IMMEDIATE_KHR)
        scores[i].supports_immediate = true;
      if (present_modes[j] == VK_PRESENT_MODE_FIFO_KHR)
        scores[i].supports_fifo = true;
    }

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count,
                                             NULL);
    VkQueueFamilyProperties* queue_family_proprties =
        malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count,
                                             queue_family_proprties);

    for (int j = 0; j < queue_family_count; j++) {
      VkBool32 supports_presentation;
      vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, context->surface,
                                           &supports_presentation);

      if ((queue_family_proprties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
          (queue_family_proprties[j].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
          supports_presentation) {
        scores[i].selected_queue = j;
        break;
      }
    }

    free(queue_family_proprties);

    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extension_count,
                                         NULL);
    VkExtensionProperties* extensions =
        malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extension_count,
                                         extensions);

    for (int j = 0; j < extension_count; j++) {
      if (strcmp(extensions[j].extensionName,
                 VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
        scores[i].supports_swapchain = true;
        break;
      }
    }

    free(extensions);

    if (selected_format != -1 && scores[i].supports_fifo &&
        scores[i].supports_immediate && scores[i].selected_queue != -1 &&
        properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU) {
      scores[i].selected_format = formats[selected_format];
      scores[i].score = 0;

      if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        scores[i].score++;
    }

    free(present_modes);
    free(formats);
  }

  int selected_device = -1;
  int highest_score = -1;

  for (int i = 0; i < device_count; i++) {
    if (scores[i].score == -1) continue;

    if (scores[i].score > highest_score) {
      highest_score = scores[i].score;
      selected_device = i;
    }
  }

  if (selected_device == -1) {
    printf("No GPU found.\n");
    free(scores);
    free(devices);
    return 0;
  }

  context->queue_family = scores[selected_device].selected_queue;
  context->surface_format = scores[selected_device].selected_format;

  VkPhysicalDeviceProperties properties;
  context->ph_device = devices[selected_device];
  vkGetPhysicalDeviceProperties(context->ph_device, &properties);
  printf("GPU: %s\nMax API version: %d.%d.%d\nCurrent API version: %d.%d.%d\n",
         properties.deviceName, VK_API_VERSION_MAJOR(properties.apiVersion),
         VK_API_VERSION_MINOR(properties.apiVersion),
         VK_API_VERSION_PATCH(properties.apiVersion),
         VK_API_VERSION_MAJOR(VK_API_VERSION_1_0),
         VK_API_VERSION_MINOR(VK_API_VERSION_1_0),
         VK_API_VERSION_PATCH(VK_API_VERSION_1_0));

  free(scores);
  free(devices);
  return 1;
}

void _tcontext_create_device(tcontext* context) {
  vkCreateDevice(
      context->ph_device,
      &(VkDeviceCreateInfo){
          .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .queueCreateInfoCount = 1,
          .pQueueCreateInfos =
              &(VkDeviceQueueCreateInfo){
                  .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0,
                  .queueFamilyIndex = context->queue_family,
                  .queueCount = 1,
                  .pQueuePriorities = &(float){1.0f}},
          .enabledLayerCount = 0,
          .ppEnabledLayerNames = NULL,
          .enabledExtensionCount = 1,
          .ppEnabledExtensionNames =
              (const char*[]){VK_KHR_SWAPCHAIN_EXTENSION_NAME}},
      NULL, &context->device);

  vkGetDeviceQueue(context->device, context->queue_family, 0, &context->queue);
}

void _tcontext_create_swapchain(tcontext* context, bool vsync) {
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->ph_device,
                                            context->surface, &capabilities);

  context->size[0] = capabilities.currentExtent.width;
  context->size[1] = capabilities.currentExtent.height;
  context->min_image_count = capabilities.minImageCount + 1;

  vkCreateSwapchainKHR(
      context->device,
      &(VkSwapchainCreateInfoKHR){
          .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
          .pNext = NULL,
          .flags = 0,
          .surface = context->surface,
          .minImageCount = context->min_image_count,
          .imageFormat = context->surface_format.format,
          .imageColorSpace = context->surface_format.colorSpace,
          .imageExtent = capabilities.currentExtent,
          .imageArrayLayers = 1,
          .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
          .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
          .queueFamilyIndexCount = 0,
          .pQueueFamilyIndices = NULL,
          .preTransform = capabilities.currentTransform,
          .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
          .presentMode =
              vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR,
          .clipped = VK_TRUE,
          .oldSwapchain = context->old_swapchain},
      NULL, &context->swapchain);
}

void _tcontext_create_renderpass(tcontext* context) {
  vkCreateRenderPass(
      context->device,
      &(VkRenderPassCreateInfo){
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
          .pNext = NULL,
          .flags = 0,
          .attachmentCount = 1,
          .pAttachments =
              &(VkAttachmentDescription){
                  .flags = 0,
                  .format = context->surface_format.format,
                  .samples = VK_SAMPLE_COUNT_1_BIT,
                  .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                  .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                  .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                  .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                  .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
          .subpassCount = 1,
          .pSubpasses =
              &(VkSubpassDescription){
                  .flags = 0,
                  .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                  .inputAttachmentCount = 0,
                  .pInputAttachments = NULL,
                  .colorAttachmentCount = 1,
                  .pColorAttachments =
                      &(VkAttachmentReference){
                          .attachment = 0,
                          .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                  .pResolveAttachments = NULL,
                  .pDepthStencilAttachment = NULL,
                  .preserveAttachmentCount = 0,
                  .pPreserveAttachments = NULL},
          .dependencyCount = 2,
          .pDependencies =
              (VkSubpassDependency[]){
                  {.srcSubpass = VK_SUBPASS_EXTERNAL,
                   .dstSubpass = 0,
                   .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                   .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                   .srcAccessMask =
                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                   .dstAccessMask =
                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                   .dependencyFlags = 0},
                  {.srcSubpass = VK_SUBPASS_EXTERNAL,
                   .dstSubpass = 0,
                   .srcStageMask =
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   .dstStageMask =
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                   .srcAccessMask = 0,
                   .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                   .dependencyFlags = 0}}},
      NULL, &context->renderpass);
}

void _tcontext_create_views(tcontext* context) {
  vkGetSwapchainImagesKHR(context->device, context->swapchain,
                          &context->image_count, NULL);
  context->swapchain_frames =
      malloc(context->image_count * sizeof(_tcontext_swapchain_frame));

  VkImage* images = malloc(context->image_count * sizeof(VkImage));
  vkGetSwapchainImagesKHR(context->device, context->swapchain,
                          &context->image_count, images);

  for (int i = 0; i < context->image_count; i++)
    context->swapchain_frames[i].image = images[i];

  free(images);

  for (int i = 0; i < context->image_count; i++) {
    vkCreateImageView(
        context->device,
        &(VkImageViewCreateInfo){
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = context->swapchain_frames[i].image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = context->surface_format.format,
            .components =
                (VkComponentMapping){.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                     .a = VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange =
                (VkImageSubresourceRange){
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1}},
        NULL, &context->swapchain_frames[i].image_view);

    vkCreateFramebuffer(
        context->device,
        &(VkFramebufferCreateInfo){
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = context->renderpass,
            .attachmentCount = 1,
            .pAttachments = &context->swapchain_frames[i].image_view,
            .width = context->size[0],
            .height = context->size[1],
            .layers = 1},
        NULL, &context->swapchain_frames[i].framebuffer);
  }
}

void _tcontext_create_frames(tcontext* context) {
  context->frames = malloc(context->fif * sizeof(tcontext_frame));
  context->render_completes =
      malloc(context->image_count * sizeof(VkSemaphore));

  for (int i = 0; i < context->fif; i++) {
    vkCreateSemaphore(context->device,
                      &(VkSemaphoreCreateInfo){
                          .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                          .pNext = NULL,
                          .flags = 0},
                      NULL, &context->frames[i].present_complete);
    vkCreateFence(
        context->device,
        &(VkFenceCreateInfo){.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                             .pNext = NULL,
                             .flags = VK_FENCE_CREATE_SIGNALED_BIT},
        NULL, &context->frames[i].wait_fence);
  }

  for (int i = 0; i < context->image_count; i++) {
    vkCreateSemaphore(context->device,
                      &(VkSemaphoreCreateInfo){
                          .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                          .pNext = NULL,
                          .flags = 0},
                      NULL, &context->render_completes[i]);
  }

  vkCreateCommandPool(
      context->device,
      &(VkCommandPoolCreateInfo){
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .pNext = NULL,
          .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
          .queueFamilyIndex = context->queue_family},
      NULL, &context->cmd_pool);

  VkCommandBuffer* cmd_buffers =
      malloc((context->fif + 1) * sizeof(VkCommandBuffer));
  vkAllocateCommandBuffers(
      context->device,
      &(VkCommandBufferAllocateInfo){
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          .pNext = NULL,
          .commandPool = context->cmd_pool,
          .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          .commandBufferCount = context->fif + 1},
      cmd_buffers);

  for (int i = 0; i < context->fif; i++)
    context->frames[i].cmd = cmd_buffers[i];

  context->transfer_cmd = cmd_buffers[context->fif];

  free(cmd_buffers);

  vkCreateFence(
      context->device,
      &(VkFenceCreateInfo){.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                           .pNext = NULL,
                           .flags = 0},
      NULL, &context->transfer_fence);
}

void _tcontext_create_allocator(tcontext* context) {
  vmaCreateAllocator(
      &(VmaAllocatorCreateInfo){.flags = 0,
                                .physicalDevice = context->ph_device,
                                .device = context->device,
                                .instance = context->instance,
                                .vulkanApiVersion = VK_API_VERSION_1_0},
      &context->allocator);
}

VkShaderModule tcontext_create_shader(tcontext* context, const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("error reading file: \'%s\'\n", filename);
    return VK_NULL_HANDLE;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  unsigned int* content = malloc(file_size);
  fread(content, sizeof(unsigned int), file_size / sizeof(unsigned int), file);
  fclose(file);

  VkShaderModule module;
  vkCreateShaderModule(context->device,
                       &(VkShaderModuleCreateInfo){
                           .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .codeSize = file_size,
                           .pCode = content,
                       },
                       NULL, &module);
  free(content);
  return module;
}

void _tcontext_create_descriptor_pool(tcontext* context) {
  vkCreateDescriptorPool(
      context->device,
      &(VkDescriptorPoolCreateInfo){
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
          .pNext = NULL,
          .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
          .maxSets = 20,
          .poolSizeCount = 2,
          .pPoolSizes =
              (VkDescriptorPoolSize[]){
                  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 20},
                  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20},
              }},
      NULL, &context->descriptor_pool);
}

tcontext* tcontext_create(twindow* window, bool vsync, int fif) {
  tcontext* context = malloc(sizeof(tcontext));
  context->fif = fif;
  context->old_swapchain = VK_NULL_HANDLE;
  context->swapchain_ok = true;
  context->current_frame = 0;

  _tcontext_create_instance(context);
  _tcontext_create_surface(context, window);
  if (!_tcontext_select_device(context)) {
    return NULL;
  }
  _tcontext_create_device(context);
  _tcontext_create_swapchain(context, vsync);
  _tcontext_create_renderpass(context);
  _tcontext_create_views(context);
  _tcontext_create_frames(context);
  _tcontext_create_allocator(context);
  _tcontext_create_descriptor_pool(context);
  return context;
}

void tcontext_resize(tcontext* context, const ivec2 size, bool vsync) {
  tcontext_wait_idle(context);
  context->old_swapchain = context->swapchain;

  for (int i = 0; i < context->image_count; i++) {
    vkDestroyFramebuffer(context->device,
                         context->swapchain_frames[i].framebuffer, NULL);
    vkDestroyImageView(context->device, context->swapchain_frames[i].image_view,
                       NULL);
  }
  free(context->swapchain_frames);

  _tcontext_create_swapchain(context, vsync);
  vkDestroySwapchainKHR(context->device, context->old_swapchain, NULL);
  _tcontext_create_views(context);

  context->swapchain_ok = true;
}

bool tcontext_begin(tcontext* context) {
  tcontext_frame* fr = context->frames + context->current_frame;
  vkWaitForFences(context->device, 1, &fr->wait_fence, VK_TRUE, UINT64_MAX);
  vkResetFences(context->device, 1, &fr->wait_fence);

  VkResult r = vkAcquireNextImageKHR(context->device, context->swapchain,
                                     UINT64_MAX, fr->present_complete,
                                     VK_NULL_HANDLE, &context->current_image);

  if (r == VK_ERROR_OUT_OF_DATE_KHR) {
    context->swapchain_ok = false;
    return false;
  }

  vkResetCommandBuffer(fr->cmd, 0);
  vkBeginCommandBuffer(fr->cmd,
                       &(VkCommandBufferBeginInfo){
                           .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                           .pNext = NULL,
                           .flags = 0,
                           .pInheritanceInfo = NULL});

  return true;
}

void tcontext_clear(tcontext* context, const vec4 clear_color) {
  tcontext_frame* fr = context->frames + context->current_frame;
  _tcontext_swapchain_frame* sfr =
      context->swapchain_frames + context->current_image;

  vkCmdBeginRenderPass(
      fr->cmd,
      &(VkRenderPassBeginInfo){
          .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
          .pNext = NULL,
          .renderPass = context->renderpass,
          .framebuffer = sfr->framebuffer,
          .renderArea = {.offset = {.x = 0, .y = 0},
                         .extent = {.width = context->size[0],
                                    .height = context->size[1]}},
          .clearValueCount = 1,
          .pClearValues =
              &(VkClearValue){
                  .color = {.float32 = {clear_color[0], clear_color[1],
                                        clear_color[2], clear_color[3]}}}},
      VK_SUBPASS_CONTENTS_INLINE);
}

void tcontext_end(tcontext* context) {
  tcontext_frame* fr = context->frames + context->current_frame;

  vkCmdEndRenderPass(fr->cmd);
  vkEndCommandBuffer(fr->cmd);

  vkQueueSubmit(
      context->queue, 1,
      &(VkSubmitInfo){.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                      .pNext = NULL,
                      .waitSemaphoreCount = 1,
                      .pWaitSemaphores = &fr->present_complete,
                      .pWaitDstStageMask =
                          &(VkPipelineStageFlags){
                              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                      .commandBufferCount = 1,
                      .pCommandBuffers = &fr->cmd,
                      .signalSemaphoreCount = 1,
                      .pSignalSemaphores =
                          &context->render_completes[context->current_image]},
      fr->wait_fence);

  VkResult r = vkQueuePresentKHR(
      context->queue,
      &(VkPresentInfoKHR){
          .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
          .pNext = NULL,
          .waitSemaphoreCount = 1,
          .pWaitSemaphores = &context->render_completes[context->current_image],
          .swapchainCount = 1,
          .pSwapchains = &context->swapchain,
          .pImageIndices = &context->current_image,
          .pResults = NULL});

  if (r == VK_ERROR_OUT_OF_DATE_KHR) {
    context->swapchain_ok = false;
  }

  context->current_frame = (context->current_frame + 1) % context->fif;
}

void tcontext_wait_idle(tcontext* context) { vkQueueWaitIdle(context->queue); }

void tcontext_destroy(tcontext* context) {
  vkDestroyDescriptorPool(context->device, context->descriptor_pool, NULL);
  vmaDestroyAllocator(context->allocator);

  vkDestroyFence(context->device, context->transfer_fence, NULL);
  vkDestroyCommandPool(context->device, context->cmd_pool, NULL);

  for (int i = 0; i < context->image_count; i++) {
    vkDestroySemaphore(context->device, context->render_completes[i], NULL);
  }

  for (int i = 0; i < context->fif; i++) {
    vkDestroyFence(context->device, context->frames[i].wait_fence, NULL);
    vkDestroySemaphore(context->device, context->frames[i].present_complete,
                       NULL);
  }

  free(context->render_completes);
  free(context->frames);

  for (int i = 0; i < context->image_count; i++) {
    vkDestroyFramebuffer(context->device,
                         context->swapchain_frames[i].framebuffer, NULL);
    vkDestroyImageView(context->device, context->swapchain_frames[i].image_view,
                       NULL);
  }

  free(context->swapchain_frames);
  vkDestroyRenderPass(context->device, context->renderpass, NULL);
  vkDestroySwapchainKHR(context->device, context->swapchain, NULL);
  vkDestroyDevice(context->device, NULL);
  vkDestroySurfaceKHR(context->instance, context->surface, NULL);
  vkDestroyInstance(context->instance, NULL);

  free(context);
}