#ifndef TCONTEXT_H
#define TCONTEXT_H

#include <vulkan/vulkan.h>

#include "../external/vma/vma.h"
#include "../framework/twindow.h"

typedef struct tcontext_frame {
  VkSemaphore present_complete;
  VkFence wait_fence;
  VkCommandBuffer cmd;
} tcontext_frame;

typedef struct _tcontext_swapchain_frame {
  VkImage image;
  VkImageView image_view;
  VkFramebuffer framebuffer;
} _tcontext_swapchain_frame;

typedef struct tcontext {
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice ph_device;
  VkDevice device;
  VkQueue queue;
  VkSwapchainKHR old_swapchain;
  VkSwapchainKHR swapchain;
  VkDescriptorPool descriptor_pool;

  int fif;
  int current_frame;
  bool swapchain_ok;
  VkCommandPool cmd_pool;
  tcontext_frame* frames;
  VkSemaphore* render_completes;
  VkCommandBuffer transfer_cmd;
  VkFence transfer_fence;

  uint32_t image_count;
  uint32_t current_image;
  uint32_t min_image_count;
  _tcontext_swapchain_frame* swapchain_frames;
  VkRenderPass renderpass;

  VmaAllocator allocator;

  uint32_t queue_family;
  VkSurfaceFormatKHR surface_format;
  ivec2 size;
} tcontext;

VkShaderModule tcontext_create_shader(tcontext* context, const char* filename);

tcontext* tcontext_create(twindow* window, bool vsync, int fif);
void tcontext_resize(tcontext* context, const ivec2 size, bool vsync);
bool tcontext_begin(tcontext* context);
void tcontext_clear(tcontext* context, const vec4 clear_color);
void tcontext_end(tcontext* context);
void tcontext_wait_idle(tcontext* context);
void tcontext_destroy(tcontext* context);

#endif