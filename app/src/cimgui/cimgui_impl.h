#ifndef CIMGUI_IMPL_DEFINED
#define CIMGUI_IMPL_DEFINED
#ifdef CIMGUI_DEFINE_ENUMS_AND_STRUCTS

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWmonitor GLFWmonitor;
struct GLFWwindow;
struct GLFWmonitor;
#endif //CIMGUI_DEFINE_ENUMS_AND_STRUCTS
CIMGUI_API bool igImplGlfw_InitForVulkan(GLFWwindow* window,bool install_callbacks);
CIMGUI_API void igImplGlfw_Shutdown(void);
CIMGUI_API void igImplGlfw_NewFrame(void);

#ifdef CIMGUI_DEFINE_ENUMS_AND_STRUCTS

typedef struct ImGui_ImplVulkanH_Frame ImGui_ImplVulkanH_Frame;
typedef struct ImGui_ImplVulkanH_Window ImGui_ImplVulkanH_Window;
typedef struct ImGui_ImplVulkan_PipelineInfo ImGui_ImplVulkan_PipelineInfo;
struct ImGui_ImplVulkan_PipelineInfo
{
    VkRenderPass RenderPass;
    uint32_t Subpass;
    VkSampleCountFlagBits MSAASamples;
    VkPipelineRenderingCreateInfoKHR PipelineRenderingCreateInfo;
    VkImageUsageFlags SwapChainImageUsage;
};
typedef struct ImGui_ImplVulkan_InitInfo ImGui_ImplVulkan_InitInfo;
struct ImGui_ImplVulkan_InitInfo
{
    uint32_t ApiVersion;
    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    uint32_t QueueFamily;
    VkQueue Queue;
    VkDescriptorPool DescriptorPool;
    uint32_t DescriptorPoolSize;
    uint32_t MinImageCount;
    uint32_t ImageCount;
    VkPipelineCache PipelineCache;
    ImGui_ImplVulkan_PipelineInfo PipelineInfoMain;
    ImGui_ImplVulkan_PipelineInfo PipelineInfoForViewports;
    bool UseDynamicRendering;
    const VkAllocationCallbacks* Allocator;
    void (*CheckVkResultFn)(VkResult err);
    VkDeviceSize MinAllocationSize;
    VkShaderModuleCreateInfo CustomShaderVertCreateInfo;
    VkShaderModuleCreateInfo CustomShaderFragCreateInfo;
};
typedef struct ImGui_ImplVulkan_RenderState ImGui_ImplVulkan_RenderState;
struct ImGui_ImplVulkan_RenderState
{
    VkCommandBuffer CommandBuffer;
    VkPipeline Pipeline;
    VkPipelineLayout PipelineLayout;
};
struct ImGui_ImplVulkanH_Frame;
struct ImGui_ImplVulkanH_Window;
struct ImGui_ImplVulkanH_Frame
{
    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;
    VkFence Fence;
    VkImage Backbuffer;
    VkImageView BackbufferView;
    VkFramebuffer Framebuffer;
};
typedef struct ImGui_ImplVulkanH_FrameSemaphores ImGui_ImplVulkanH_FrameSemaphores;
struct ImGui_ImplVulkanH_FrameSemaphores
{
    VkSemaphore ImageAcquiredSemaphore;
    VkSemaphore RenderCompleteSemaphore;
};
typedef struct ImVector_ImGui_ImplVulkanH_Frame {int Size;int Capacity;ImGui_ImplVulkanH_Frame* Data;} ImVector_ImGui_ImplVulkanH_Frame;

typedef struct ImVector_ImGui_ImplVulkanH_FrameSemaphores {int Size;int Capacity;ImGui_ImplVulkanH_FrameSemaphores* Data;} ImVector_ImGui_ImplVulkanH_FrameSemaphores;

struct ImGui_ImplVulkanH_Window
{
    int Width;
    int Height;
    VkSwapchainKHR Swapchain;
    VkSurfaceKHR Surface;
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    VkRenderPass RenderPass;
    bool UseDynamicRendering;
    bool ClearEnable;
    VkClearValue ClearValue;
    uint32_t FrameIndex;
    uint32_t ImageCount;
    uint32_t SemaphoreCount;
    uint32_t SemaphoreIndex;
    ImVector_ImGui_ImplVulkanH_Frame Frames;
    ImVector_ImGui_ImplVulkanH_FrameSemaphores FrameSemaphores;
};
#endif //CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#ifndef CIMGUI_DEFINE_ENUMS_AND_STRUCTS
typedef ImVector<ImGui_ImplVulkanH_Frame> ImVector_ImGui_ImplVulkanH_Frame;
typedef ImVector<ImGui_ImplVulkanH_FrameSemaphores> ImVector_ImGui_ImplVulkanH_FrameSemaphores;
#endif //CIMGUI_DEFINE_ENUMS_AND_STRUCTS
CIMGUI_API bool igImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info);
CIMGUI_API void igImplVulkan_Shutdown(void);
CIMGUI_API void igImplVulkan_NewFrame(void);
CIMGUI_API void igImplVulkan_RenderDrawData(ImDrawData* draw_data,VkCommandBuffer command_buffer,VkPipeline pipeline);
CIMGUI_API void igImplVulkan_UpdateTexture(ImTextureData* tex);
CIMGUI_API VkDescriptorSet igImplVulkan_AddTexture(VkSampler sampler,VkImageView image_view,VkImageLayout image_layout);
CIMGUI_API void igImplVulkan_RemoveTexture(VkDescriptorSet descriptor_set);

#endif //CIMGUI_IMPL_DEFINED