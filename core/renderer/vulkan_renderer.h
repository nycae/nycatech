//
// Created by rplaz on 2023-12-14.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "lib/types.h"
#include "obj_model.h"
#include "shader.h"

namespace NycaTech::Renderer {

class VulkanRenderer final {
public:
   VulkanRenderer();
  ~VulkanRenderer();

                  VulkanRenderer(VulkanRenderer&&) = delete;
                  VulkanRenderer(const VulkanRenderer&) = delete;
  VulkanRenderer& operator=(VulkanRenderer&&) = delete;
  VulkanRenderer& operator=(const VulkanRenderer&) = delete;

  bool AttachShader(const Shader& shader);
  bool LoadModel(ObjModel* model);
  bool DrawFrame();

public:
  inline static const Vector Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef DEBUG
  inline static const Vector Layers{ "VK_LAYER_KHRONOS_validation" };
#endif

public:
  SDL_Window*            window;
  VkInstance             instance;
  VkPhysicalDevice       physicalDevice;
  VkSurfaceKHR           surface;
  VkDevice               device;
  Uint32                 graphicsQueueIndex;
  Uint32                 presentQueueIndex;
  VkQueue                graphicsQueue;
  VkQueue                presentQueue;
  VkSwapchainKHR         swapchain;
  Vector<VkImage>        images;
  Vector<VkImageView>    imageViews;
  VkSurfaceFormatKHR     format;
  VkExtent2D             extent;
  VkDescriptorSetLayout  layout;
  VkPipelineLayout       pipelineLayout;
  VkPipeline             pipeline;
  VkRenderPass           renderPass;
  Vector<VkFramebuffer>  frameBuffer;
  VkCommandPool          commandPool;
  VkCommandBuffer        command;
  VkSemaphore            imageMutex;
  VkSemaphore            renderMutex;
  VkFence                inFlightFence;
  Vector<ObjModel*>      models;
  Vector<VkShaderModule> vertexShaders;
  Vector<VkShaderModule> fragmentShaders;
  VkBuffer               uniformBuffers[3];
  VkDeviceMemory         uniformBuffersMemory[3];
  void*                  uniformBuffersMapped[3];

private:
  bool               SetupWindow();
  bool               CreateInstance();
  bool               CreateSurface();
  bool               CreatePhysicalDevice();
  bool               IsDeviceSuitable(VkPhysicalDevice);
  Vector<Uint32>     PresentationQueueIndices() const;
  Vector<Uint32>     GraphicsQueueIndices() const;
  bool               CreateLogicalDevice();
  bool               CreateSwapChain();
  VkSurfaceFormatKHR ChooseFormat(const Vector<VkSurfaceFormatKHR>& formats);
  VkPresentModeKHR   ChooseMode(const Vector<VkPresentModeKHR>& modes);
  bool               CreateImageViews();
  bool               CreateFrameBuffers();
  bool               CreateSynch();
  bool               RecordCommandBuffer(VkCommandBuffer, Uint32);
  bool               CreateRenderPass();
  bool               CreateRenderPipeline();
  bool               CreateCommandPool();
  bool               CreateCommandBuffers();
  bool               RecreateSwapChain();
  bool               CreateUniformBuffers();

  bool     CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
  VkBuffer CreateBuffer(VkDeviceSize          size,
                        VkBufferUsageFlags    usage,
                        VkMemoryPropertyFlags properties,
                        VkDeviceMemory&       bufferMemory);
  bool     CreateVertexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data);
  bool     CreateIndexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data);
};

}  // namespace NycaTech::Renderer

#endif  // VULKAN_RENDERER_H
