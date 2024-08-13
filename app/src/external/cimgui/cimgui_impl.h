#ifdef CIMGUI_USE_GLFW

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWmonitor GLFWmonitor;
struct GLFWwindow;
struct GLFWmonitor;CIMGUI_API bool ImGui_ImplGlfw_InitForOpenGL(GLFWwindow* window,bool install_callbacks);
CIMGUI_API bool ImGui_ImplGlfw_InitForVulkan(GLFWwindow* window,bool install_callbacks);
CIMGUI_API bool ImGui_ImplGlfw_InitForOther(GLFWwindow* window,bool install_callbacks);
CIMGUI_API void ImGui_ImplGlfw_Shutdown(void);
CIMGUI_API void ImGui_ImplGlfw_NewFrame(void);
CIMGUI_API void ImGui_ImplGlfw_InstallCallbacks(GLFWwindow* window);
CIMGUI_API void ImGui_ImplGlfw_RestoreCallbacks(GLFWwindow* window);
CIMGUI_API void ImGui_ImplGlfw_SetCallbacksChainForAllWindows(bool chain_for_all_windows);
CIMGUI_API void ImGui_ImplGlfw_WindowFocusCallback(GLFWwindow* window,int focused);
CIMGUI_API void ImGui_ImplGlfw_CursorEnterCallback(GLFWwindow* window,int entered);
CIMGUI_API void ImGui_ImplGlfw_CursorPosCallback(GLFWwindow* window,double x,double y);
CIMGUI_API void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window,int button,int action,int mods);
CIMGUI_API void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window,double xoffset,double yoffset);
CIMGUI_API void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
CIMGUI_API void ImGui_ImplGlfw_CharCallback(GLFWwindow* window,unsigned int c);
CIMGUI_API void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor* monitor,int event);

#endif
#ifdef CIMGUI_USE_VULKAN

typedef struct ImGui_ImplVulkanH_Frame ImGui_ImplVulkanH_Frame;
typedef struct ImGui_ImplVulkanH_Window ImGui_ImplVulkanH_Window;
typedef struct ImGui_ImplVulkan_InitInfo ImGui_ImplVulkan_InitInfo;
struct ImGui_ImplVulkan_InitInfo
{
    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    uint32_t QueueFamily;
    VkQueue Queue;
    VkDescriptorPool DescriptorPool;
    VkRenderPass RenderPass;
    uint32_t MinImageCount;
    uint32_t ImageCount;
    VkSampleCountFlagBits MSAASamples;
    VkPipelineCache PipelineCache;
    uint32_t Subpass;
    bool UseDynamicRendering;
    VkPipelineRenderingCreateInfoKHR PipelineRenderingCreateInfo;
    const VkAllocationCallbacks* Allocator;
    void (*CheckVkResultFn)(VkResult err);
    VkDeviceSize MinAllocationSize;
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
struct ImGui_ImplVulkanH_Window
{
    int Width;
    int Height;
    VkSwapchainKHR Swapchain;
    VkSurfaceKHR Surface;
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    VkRenderPass RenderPass;
    VkPipeline Pipeline;
    bool UseDynamicRendering;
    bool ClearEnable;
    VkClearValue ClearValue;
    uint32_t FrameIndex;
    uint32_t ImageCount;
    uint32_t SemaphoreCount;
    uint32_t SemaphoreIndex;
    ImGui_ImplVulkanH_Frame* Frames;
    ImGui_ImplVulkanH_FrameSemaphores* FrameSemaphores;
};CIMGUI_API bool ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info);
CIMGUI_API void ImGui_ImplVulkan_Shutdown(void);
CIMGUI_API void ImGui_ImplVulkan_NewFrame(void);
CIMGUI_API void ImGui_ImplVulkan_RenderDrawData(ImDrawData* draw_data,VkCommandBuffer command_buffer,VkPipeline pipeline);
CIMGUI_API bool ImGui_ImplVulkan_CreateFontsTexture(void);
CIMGUI_API void ImGui_ImplVulkan_DestroyFontsTexture(void);
CIMGUI_API void ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count);
CIMGUI_API VkDescriptorSet ImGui_ImplVulkan_AddTexture(VkSampler sampler,VkImageView image_view,VkImageLayout image_layout);
CIMGUI_API void ImGui_ImplVulkan_RemoveTexture(VkDescriptorSet descriptor_set);
CIMGUI_API bool ImGui_ImplVulkan_LoadFunctions(PFN_vkVoidFunction(*loader_func)(const char* function_name,void* user_data),void* user_data);
CIMGUI_API void ImGui_ImplVulkanH_CreateOrResizeWindow(VkInstance instance,VkPhysicalDevice physical_device,VkDevice device,ImGui_ImplVulkanH_Window* wnd,uint32_t queue_family,const VkAllocationCallbacks* allocator,int w,int h,uint32_t min_image_count);
CIMGUI_API void ImGui_ImplVulkanH_DestroyWindow(VkInstance instance,VkDevice device,ImGui_ImplVulkanH_Window* wnd,const VkAllocationCallbacks* allocator);
CIMGUI_API VkSurfaceFormatKHR ImGui_ImplVulkanH_SelectSurfaceFormat(VkPhysicalDevice physical_device,VkSurfaceKHR surface,const VkFormat* request_formats,int request_formats_count,VkColorSpaceKHR request_color_space);
CIMGUI_API VkPresentModeKHR ImGui_ImplVulkanH_SelectPresentMode(VkPhysicalDevice physical_device,VkSurfaceKHR surface,const VkPresentModeKHR* request_modes,int request_modes_count);
CIMGUI_API int ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);

#endif
