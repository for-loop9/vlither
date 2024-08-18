#include "ig_context.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ig_buffer.h"
#include "ig_texture.h"
#include "../math/ig_mat4.h"

VkShaderModule ig_context_create_shader_from_file(ig_context* context, const char* file_name) {
	FILE* file = fopen(file_name, "rb");
	if (!file) {
		printf("error reading file: \'%s\'\n", file_name);
		exit(-1);
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	unsigned int* content = malloc(file_size);
	fread(content, sizeof(unsigned int), file_size / sizeof(unsigned int), file);
	fclose(file);

	VkShaderModule r;
	vkCreateShaderModule(context->device, &(VkShaderModuleCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = file_size,
		.pCode = content,
	}, NULL, &r);
	free(content);
	return r;
}

ig_context* ig_context_create(ig_window* window, const ig_ivec2* resolution, int vsync) {
	ig_context* r = malloc(sizeof(ig_context));
	r->gpu = VK_NULL_HANDLE;
	r->fif = 2;
	r->frame_idx = 0;
	r->default_frame.resolution = *resolution;

	unsigned int extensions_count;
	const char** instance_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

	vkCreateInstance(&(VkInstanceCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.pApplicationInfo = &(VkApplicationInfo) {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = NULL,
			.pApplicationName = "app",
			.applicationVersion = 0,
			.pEngineName = "ignite",
			.apiVersion = VK_API_VERSION_1_0
		},
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = extensions_count,
		.ppEnabledExtensionNames = instance_extensions
	}, NULL, &r->instance);

	glfwCreateWindowSurface(r->instance, window->native_handle, NULL, &r->surface);
	
	unsigned int device_count;
	vkEnumeratePhysicalDevices(r->instance, &device_count, NULL);
	VkPhysicalDevice* devices = malloc(device_count * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(r->instance, &device_count, devices);
	VkSurfaceCapabilitiesKHR capabilities;

	int selected_queue;

	const char* swapchain_ext = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	for (int i = 0; i < device_count; i++) {
		selected_queue = -1;
		int selected_format = -1;
		int supports_swapchain = 0;
		int supports_fifo = 0;
		int supports_immediate = 0;

		unsigned int format_count;
		unsigned int queue_count;
		unsigned int present_mode_count;

		VkPhysicalDeviceProperties device_props;
		vkGetPhysicalDeviceProperties(devices[i], &device_props);
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], r->surface, &capabilities);
		r->surface_extent = capabilities.currentExtent;

		if (r->surface_extent.width != window->dim.x || r->surface_extent.height != window->dim.y) {
			printf("extent does not match window size, window = [%d, %d], extent = [%d, %d]\n", window->dim.x, window->dim.y, r->surface_extent.width, r->surface_extent.height);
			exit(-1);
		}

		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], r->surface, &format_count, NULL);
		VkSurfaceFormatKHR* formats = malloc(format_count * sizeof(VkSurfaceFormatKHR));
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], r->surface, &format_count, formats);
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_count, NULL);
		VkQueueFamilyProperties* queues = malloc(queue_count * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_count, queues);
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensions_count, NULL);
		VkExtensionProperties* device_extensions = malloc(extensions_count * sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensions_count, device_extensions);
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], r->surface, &present_mode_count, NULL);
		VkPresentModeKHR* present_modes = malloc(present_mode_count * sizeof(VkPresentModeKHR));
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], r->surface, &present_mode_count, present_modes);
		for (int j = 0; j < present_mode_count; j++) {
			if (present_modes[j] == VK_PRESENT_MODE_FIFO_KHR) {
				supports_fifo = 1;
				break;
			}
		}
		for (int j = 0; j < present_mode_count; j++) {
			if (present_modes[j] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				supports_immediate = 1;
				break;
			}
		}
		free(present_modes);

		for (int j = 0; j < extensions_count; j++) {
			if (strcmp(device_extensions[j].extensionName, swapchain_ext) == 0) {
				supports_swapchain = 1;
				break;
			}
		}
		free(device_extensions);

		for (int j = 0; j < queue_count; j++) {
			VkBool32 presentation = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, r->surface, &presentation);
			if ((queues[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queues[j].queueFlags & VK_QUEUE_COMPUTE_BIT) && presentation) {
				selected_queue = j;
				break;
			}
		}

		for (int j = 0; j < format_count; j++) {
			if (formats[j].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && formats[j].format == VK_FORMAT_B8G8R8A8_UNORM) {
				selected_format = j;
				break;
			}
		}
		if (selected_queue != -1 && selected_format != -1 && supports_swapchain && supports_fifo && supports_immediate) {
			r->queue_family = selected_queue;
			r->surface_format = formats[selected_format];
			r->gpu = devices[i];
			printf("selected card: %s, max api version %d.%d.%d, running %d.%d.%d\n", device_props.deviceName,
				VK_API_VERSION_MAJOR(device_props.apiVersion),
				VK_API_VERSION_MINOR(device_props.apiVersion),
				VK_API_VERSION_PATCH(device_props.apiVersion),
				VK_API_VERSION_MAJOR(VK_API_VERSION_1_0),
				VK_API_VERSION_MINOR(VK_API_VERSION_1_0),
				VK_API_VERSION_PATCH(VK_API_VERSION_1_0));
			free(queues);
			free(formats);
			break;
		}
	}
	free(devices);

	if (r->gpu == VK_NULL_HANDLE) {
		printf("no suitable card found\n");
		exit(-1);
	}

	vkCreateDevice(r->gpu, &(VkDeviceCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.queueFamilyIndex = selected_queue,
			.pQueuePriorities = (float[]) { 1 },
			.queueCount = 1
		},
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = &swapchain_ext,
		.pEnabledFeatures = NULL
	}, NULL, &r->device);

	vkGetDeviceQueue(r->device, selected_queue, 0, &r->queue);

	vkCreateSwapchainKHR(r->device, &(VkSwapchainCreateInfoKHR) {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = r->surface,
		.minImageCount = capabilities.minImageCount + 1,
		.imageFormat = r->surface_format.format,
		.imageColorSpace = r->surface_format.colorSpace,
		.imageExtent = r->surface_extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &selected_queue,
		.preTransform = capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR,
		.clipped = VK_FALSE,
		.oldSwapchain = VK_NULL_HANDLE
	}, NULL, &r->swapchain);

	vkCreateCommandPool(r->device, &(VkCommandPoolCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = selected_queue
	}, NULL, &r->cmd_pool);

	VkCommandBuffer* cmd_buffers = malloc((r->fif + 1) * sizeof(VkCommandBuffer));
	vkAllocateCommandBuffers(r->device, &(VkCommandBufferAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = r->cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = r->fif + 1
	}, cmd_buffers);

	r->frames = malloc(r->fif * sizeof(_ig_frame));
	for (int i = 0; i < r->fif; i++) {
		vkCreateSemaphore(r->device, &(VkSemaphoreCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0
		}, NULL, &r->frames[i].image_available);
		vkCreateSemaphore(r->device, &(VkSemaphoreCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0
		}, NULL, &r->frames[i].render_finished);
		vkCreateFence(r->device, &(VkFenceCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		}, NULL, &r->frames[i].frame_finished);
		r->frames[i].cmd_buffer = cmd_buffers[i];
	}
	r->transfer_cmd_buffer = cmd_buffers[r->fif];
	free(cmd_buffers);

	vkCreateRenderPass(r->device, &(VkRenderPassCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.attachmentCount = 1,
		.pAttachments = &(VkAttachmentDescription) {
			.flags = 0,
			.format = r->surface_format.format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		},
		.subpassCount = 1,
		.pSubpasses = &(VkSubpassDescription) {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = NULL,
			.colorAttachmentCount = 1,
			.pColorAttachments = &(VkAttachmentReference) {
				.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			},
			.pResolveAttachments = NULL,
			.pDepthStencilAttachment = NULL,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = NULL
		},
		.dependencyCount = 0,
		.pDependencies = NULL,
	}, NULL, &r->swapchain_pass);

	vkGetSwapchainImagesKHR(r->device, r->swapchain, &r->swapchain_frame_count, NULL);
	VkImage* swapchain_images = malloc(r->swapchain_frame_count * sizeof(VkImage));
	vkGetSwapchainImagesKHR(r->device, r->swapchain, &r->swapchain_frame_count, swapchain_images);

	r->swapchain_frames = malloc(r->swapchain_frame_count * sizeof(_ig_swapchain_frame));

	for (int i = 0; i < r->swapchain_frame_count; i++) {
		vkCreateImageView(r->device, &(VkImageViewCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = swapchain_images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = r->surface_format.format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = (VkImageSubresourceRange) {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		}, NULL, &r->swapchain_frames[i].color_attachment);

		vkCreateFramebuffer(r->device, &(VkFramebufferCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.renderPass = r->swapchain_pass,
			.attachmentCount = 1,
			.pAttachments = &r->swapchain_frames[i].color_attachment,
			.width = r->surface_extent.width,
			.height = r->surface_extent.height,
			.layers = 1
		}, NULL, &r->swapchain_frames[i].framebuffer);
	}
	free(swapchain_images);

	vkCreateDescriptorPool(r->device, &(VkDescriptorPoolCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.maxSets = 10,
		.poolSizeCount = 2,
		.pPoolSizes = (VkDescriptorPoolSize[]) {
			{
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 10
			},
			{
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 10
			}
		}
	}, NULL, &r->descriptor_pool);

	vkCreateDescriptorSetLayout(r->device, &(VkDescriptorSetLayoutCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.bindingCount = 2,
		.pBindings = (VkDescriptorSetLayoutBinding[]) {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pImmutableSamplers = NULL
			},
			{
				.binding = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				.pImmutableSamplers = NULL
			}
		}
	}, NULL, &r->global_layout);

	vkCreateDescriptorSetLayout(r->device, &(VkDescriptorSetLayoutCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = (VkDescriptorSetLayoutBinding[]) {
			{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				.pImmutableSamplers = NULL
			}
		}
	}, NULL, &r->texture_layout);

	vkCreatePipelineLayout(r->device, &(VkPipelineLayoutCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.setLayoutCount = 2,
		.pSetLayouts = (VkDescriptorSetLayout[]) { r->global_layout, r->texture_layout },
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	}, NULL, &r->standard_layout);

	VkShaderModule vertex_shader = ig_context_create_shader_from_file(r, "ignite/res/shaders/swapchainv.spv");
	VkShaderModule fragment_shader = ig_context_create_shader_from_file(r, "ignite/res/shaders/swapchainf.spv");

	vkCreateGraphicsPipelines(r->device, VK_NULL_HANDLE, 1, &(VkGraphicsPipelineCreateInfo) {
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
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &(VkVertexInputBindingDescription) {
				.binding = 0,
				.stride = 2 * sizeof(float),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			},
			.vertexAttributeDescriptionCount = 1,
			.pVertexAttributeDescriptions = &(VkVertexInputAttributeDescription) {
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = 0
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
				.width = (float) r->surface_extent.width,
				.height = (float) r->surface_extent.height,
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			},
			.scissorCount = 1,
			.pScissors = &(VkRect2D) {
				.offset = { .x = 0, .y = 0 },
				.extent = r->surface_extent
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
				.blendEnable = VK_FALSE,
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
		.layout = r->standard_layout,
		.renderPass = r->swapchain_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	}, NULL, &r->swapchain_pipeline);
	
	vkDestroyShaderModule(r->device, fragment_shader, NULL);
	vkDestroyShaderModule(r->device, vertex_shader, NULL);

	vmaCreateAllocator(&(VmaAllocatorCreateInfo) {
		.flags = 0,
		.physicalDevice = r->gpu,
		.device = r->device,
		.preferredLargeHeapBlockSize = 0,
		.pAllocationCallbacks = NULL,
		.pDeviceMemoryCallbacks = NULL,
		.pHeapSizeLimit = 0,
		.pVulkanFunctions = NULL,
		.instance = r->instance,
		.vulkanApiVersion = VK_API_VERSION_1_0,
		.pTypeExternalMemoryHandleTypes = NULL
	}, &r->allocator);

	float quad_data[] = {
		-1, -1,
		-1, 1,
		1, -1,
		1, 1,
	};

	vkCreateFence(r->device, &(VkFenceCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	}, NULL, &r->transfer_fence);

	r->quad_buffer = ig_context_buffer_create(r, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, quad_data, sizeof(quad_data));

	vkCreateRenderPass(r->device, &(VkRenderPassCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.attachmentCount = 2,
		.pAttachments = (VkAttachmentDescription[]) {
			{
				.flags = 0,
				.format = r->surface_format.format,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			}, {
				.flags = 0,
				.format = VK_FORMAT_D32_SFLOAT,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}
		},
		.subpassCount = 1,
		.pSubpasses = &(VkSubpassDescription) {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = NULL,
			.colorAttachmentCount = 1,
			.pColorAttachments = &(VkAttachmentReference) {
				.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			},
			.pResolveAttachments = NULL,
			.pDepthStencilAttachment = &(VkAttachmentReference) {
				.attachment = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			},
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = NULL
		},
		.dependencyCount = 1,
		.pDependencies = &(VkSubpassDependency) {
			.srcSubpass = 0,
			.dstSubpass = VK_SUBPASS_EXTERNAL,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
		},
	}, NULL, &r->default_frame.render_pass);

	vmaCreateImage(r->allocator, &(VkImageCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = r->surface_format.format,
		.extent = {
			.width = r->default_frame.resolution.x,
			.height = r->default_frame.resolution.y,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	}, &(VmaAllocationCreateInfo) {
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = VK_NULL_HANDLE,
		.pUserData = NULL,
		.priority = 0
	}, &r->default_frame.color_attachment, &r->default_frame.color_attachment_memory, NULL);

	vkCreateImageView(r->device, &(VkImageViewCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = r->default_frame.color_attachment,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = r->surface_format.format,
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
	}, NULL, &r->default_frame.color_attachment_view);

	vmaCreateImage(r->allocator, &(VkImageCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.extent = {
			.width = r->default_frame.resolution.x,
			.height = r->default_frame.resolution.y,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
	}, &(VmaAllocationCreateInfo) {
		.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		.requiredFlags = 0,
		.preferredFlags = 0,
		.memoryTypeBits = 0,
		.pool = VK_NULL_HANDLE,
		.pUserData = NULL,
		.priority = 0
	}, &r->default_frame.depth_attachment, &r->default_frame.depth_attachment_memory, NULL);

	vkCreateImageView(r->device, &(VkImageViewCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = r->default_frame.depth_attachment,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	}, NULL, &r->default_frame.depth_attachment_view);

	vkCreateFramebuffer(r->device, &(VkFramebufferCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.renderPass = r->default_frame.render_pass,
		.attachmentCount = 2,
		.pAttachments = (VkImageView[]) { r->default_frame.color_attachment_view, r->default_frame.depth_attachment_view },
		.width = r->default_frame.resolution.x,
		.height = r->default_frame.resolution.y,
		.layers = 1
	}, NULL, &r->default_frame.framebuffer);

	vkCreateSampler(r->device, &(VkSamplerCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.magFilter = VK_FILTER_NEAREST,
		.minFilter = VK_FILTER_NEAREST,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 0.0f,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	}, NULL, &r->nearest_sampler);

	vkCreateSampler(r->device, &(VkSamplerCreateInfo) {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 0.0f,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	}, NULL, &r->linear_sampler);

	vkAllocateDescriptorSets(r->device, &(VkDescriptorSetAllocateInfo) {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = r->descriptor_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &r->global_layout
	}, &r->global_set);

	vkUpdateDescriptorSets(r->device, 1, (VkWriteDescriptorSet[]) {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = r->global_set,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &(VkDescriptorImageInfo) {
				.sampler = r->nearest_sampler,
				.imageView = r->default_frame.color_attachment_view,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			},
			.pBufferInfo = NULL,
			.pTexelBufferView = NULL
		},

	}, 0, NULL);
	
	return r;
}

void ig_context_begin(ig_context* context) {
	_ig_frame* current_frame = context->frames + context->frame_idx;
	vkWaitForFences(context->device, 1, &current_frame->frame_finished, VK_TRUE, UINT64_MAX);
	vkResetFences(context->device, 1, &current_frame->frame_finished);
	vkAcquireNextImageKHR(context->device, context->swapchain, UINT64_MAX, current_frame->image_available, VK_NULL_HANDLE, &context->img_idx);

	vkResetCommandBuffer(current_frame->cmd_buffer, 0);
	vkBeginCommandBuffer(current_frame->cmd_buffer, &(VkCommandBufferBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	});
	vkCmdBindDescriptorSets(current_frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->standard_layout, 0, 1, &context->global_set, 0, NULL);
}

void ig_context_end(ig_context* context) {
	_ig_frame* current_frame = context->frames + context->frame_idx;

	vkCmdBindDescriptorSets(current_frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->standard_layout, 0, 1, &context->global_set, 0, NULL);
	vkCmdBindVertexBuffers(current_frame->cmd_buffer, 0, 1, &context->quad_buffer->buffer, (VkDeviceSize[]) { 0 });
	vkCmdBeginRenderPass(current_frame->cmd_buffer, &(VkRenderPassBeginInfo) {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = context->swapchain_pass,
		.framebuffer = context->swapchain_frames[context->img_idx].framebuffer,
		.renderArea = {
			.offset = { .x = 0, .y = 0 },
			.extent = context->surface_extent
		},
		.clearValueCount = 1,
		.pClearValues = &(VkClearValue) {
			.color = { .float32 = { 0.03f, 0.03f, 0.03f, 1.0f } }
		}
	}, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(current_frame->cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->swapchain_pipeline);
	vkCmdDraw(current_frame->cmd_buffer, 4, 1, 0, 0);
	vkCmdEndRenderPass(current_frame->cmd_buffer);
	
	vkEndCommandBuffer(current_frame->cmd_buffer);
	
	vkQueueSubmit(context->queue, 1, &(VkSubmitInfo) {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &current_frame->image_available,
		.pWaitDstStageMask = (VkPipelineStageFlags[]) { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT },
		.commandBufferCount = 1,
		.pCommandBuffers = &current_frame->cmd_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &current_frame->render_finished
	}, current_frame->frame_finished);

	vkQueuePresentKHR(context->queue, &(VkPresentInfoKHR) {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &current_frame->render_finished,
		.swapchainCount = 1,
		.pSwapchains = &context->swapchain,
		.pImageIndices = &context->img_idx,
		.pResults = NULL
	});

	context->frame_idx = (context->frame_idx + 1) % context->fif;
}

void ig_context_finish(ig_context * context) {
	if (context->fif != 1) {
		VkFence* wait_fences = malloc((context->fif - 1) * sizeof(VkFence));
		int fence_index = 0;
		for (int i = 0; i < context->fif; i++) {
			if (i != context->frame_idx)
				wait_fences[fence_index++] = context->frames[i].frame_finished;
		}
		vkWaitForFences(context->device, context->fif - 1, wait_fences, VK_TRUE, UINT64_MAX);
		free(wait_fences);
	} else
		vkWaitForFences(context->device, 1, &context->frames->frame_finished, VK_TRUE, UINT64_MAX);
}

void ig_context_destroy(ig_context* context) {
	vkDestroySampler(context->device, context->linear_sampler, NULL);
	vkDestroySampler(context->device, context->nearest_sampler, NULL);

	// default frame:
	vkDestroyFramebuffer(context->device, context->default_frame.framebuffer, NULL);
	vkDestroyImageView(context->device, context->default_frame.depth_attachment_view, NULL);
	vmaDestroyImage(context->allocator, context->default_frame.depth_attachment, context->default_frame.depth_attachment_memory);
	vkDestroyImageView(context->device, context->default_frame.color_attachment_view, NULL);
	vmaDestroyImage(context->allocator, context->default_frame.color_attachment, context->default_frame.color_attachment_memory);
	vkDestroyRenderPass(context->device, context->default_frame.render_pass, NULL);
	
	vkDestroyFence(context->device, context->transfer_fence, NULL);
	ig_context_buffer_destroy(context, context->quad_buffer);
	vmaDestroyAllocator(context->allocator);

	vkDestroyPipeline(context->device, context->swapchain_pipeline, NULL);
	vkDestroyPipelineLayout(context->device, context->standard_layout, NULL);
	vkDestroyDescriptorSetLayout(context->device, context->texture_layout, NULL);
	vkDestroyDescriptorSetLayout(context->device, context->global_layout, NULL);
	vkDestroyDescriptorPool(context->device, context->descriptor_pool, NULL);

	for (int i = context->swapchain_frame_count - 1; i >= 0; i--) {
		vkDestroyFramebuffer(context->device, context->swapchain_frames[i].framebuffer, NULL);
		vkDestroyImageView(context->device, context->swapchain_frames[i].color_attachment, NULL);
	}
	free(context->swapchain_frames);
	vkDestroyRenderPass(context->device, context->swapchain_pass, NULL);
	for (int i = context->fif - 1; i >= 0; i--) {
		vkDestroyFence(context->device, context->frames[i].frame_finished, NULL);
		vkDestroySemaphore(context->device, context->frames[i].render_finished, NULL);
		vkDestroySemaphore(context->device, context->frames[i].image_available, NULL);
	}
	free(context->frames);
	vkDestroyCommandPool(context->device, context->cmd_pool, NULL);
	vkDestroySwapchainKHR(context->device, context->swapchain, NULL);
	vkDestroyDevice(context->device, NULL);
	vkDestroySurfaceKHR(context->instance, context->surface, NULL);
	vkDestroyInstance(context->instance, NULL);
	free(context);
}
