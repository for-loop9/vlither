#ifndef IG_CONTEXT_H
#define IG_CONTEXT_H

#include <vulkan/vulkan.h>
#include "../framework/ig_window.h"
#include "../external/vma/vma.h"

typedef struct ig_buffer ig_buffer;
typedef struct ig_texture ig_texture;

typedef struct _ig_frame {
	VkSemaphore image_available;
	VkSemaphore render_finished;
	VkFence frame_finished;
	VkCommandBuffer cmd_buffer;
} _ig_frame;

typedef struct _ig_swapchain_frame {
	VkImageView color_attachment;
	VkFramebuffer framebuffer;
} _ig_swapchain_frame;

typedef struct ig_context {
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice gpu;
	VkSurfaceFormatKHR surface_format;
	VkExtent2D surface_extent;
	VkDevice device;
	unsigned int queue_family;
	VkQueue queue;
	VkSwapchainKHR swapchain;
	VkRenderPass swapchain_pass;
	_ig_swapchain_frame* swapchain_frames;
	unsigned int swapchain_frame_count;
	int fif;
	_ig_frame* frames;
	int frame_idx;
	unsigned int img_idx;
	VkCommandPool cmd_pool;
	VkCommandBuffer transfer_cmd_buffer;
	VkFence transfer_fence;

	VkPipelineLayout standard_layout;
	VkPipeline swapchain_pipeline;
	VmaAllocator allocator;
	ig_buffer* quad_buffer;

	struct {
		ig_ivec2 resolution;
		VkRenderPass render_pass;
		VkImage color_attachment;
		VmaAllocation color_attachment_memory;
		VkImageView color_attachment_view;
		VkImage depth_attachment;
		VmaAllocation depth_attachment_memory;
		VkImageView depth_attachment_view;
		VkFramebuffer framebuffer;
	} default_frame;

	VkSampler nearest_sampler;
	VkSampler linear_sampler;
	VkDescriptorPool descriptor_pool;
	VkDescriptorSetLayout global_layout;
	VkDescriptorSetLayout texture_layout;
	VkDescriptorSet global_set;
} ig_context;

VkShaderModule ig_context_create_shader_from_file(ig_context* context, const char* file_name);
ig_context* ig_context_create(ig_window* window, const ig_ivec2* resolution, int vsync);
void ig_context_begin(ig_context* context);
void ig_context_end(ig_context* context);
void ig_context_finish(ig_context* context);
void ig_context_destroy(ig_context* context);

#endif