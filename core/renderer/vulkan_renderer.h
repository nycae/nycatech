//
// Created by rplaz on 2023-12-14.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "obj_model.h"
#include "physical_device.h"
#include "shader.h"
#include "types.h"

namespace NycaTech::Renderer {

class VulkanRenderer final {
public:
       VulkanRenderer(SDL_Window* window);
  bool AttachShader(Shader* shader);
  bool LoadModel(ObjModel* model);
  bool PrepareRendering();
  bool DrawFrame();

public:
  static VulkanRenderer* Create(PhysicalDevice* physicalDevice);

public:
  VkInstance            instance;
  VkPhysicalDevice      physicalDevice;
  VkDevice              device;
  Uint32                graphicsQueueIndex;
  VkQueue               graphicsQueue;
  Uint32                presentQueueIndex;
  VkQueue               presentQueue;
  VkSurfaceKHR          surface;
  VkSwapchainKHR        swapchain;
  Vector<VkImage>       queuedFrames;
  Vector<VkImageView>   queuedFrameViews;
  VkFormat              imageFormat;
  VkExtent2D            extent;
  VkPipelineLayout      pipelineLayout;
  VkPipeline            pipeline;
  VkRenderPass          renderPass;
  Vector<VkFramebuffer> frameBuffer;
  VkCommandPool         commandPool;
  VkCommandBuffer       command;
  VkSemaphore           imageMutex;
  VkSemaphore           renderMutex;
  VkFence               inFlightFence;
  Vector<ObjModel*>     models;
  Vector<Shader*>       shaders;
  Uint32                currentFrame;

private:
  bool CreateInstance(SDL_Window* window);
  bool ChoosePhysicalDevice();
  bool CreateDevice();
  bool SuitableQueuesFound();
  bool CreateSwapChain();
  bool SetupImageViews();
  bool CreateFrameBuffers();
  bool CreateSynch();
  bool RecordCommandBuffer(VkCommandBuffer, Uint32);
  bool CreateRenderPass();
  bool CreateRenderPipeline();
  bool CreateCommandPool();
  bool CreateCommandBuffers();
  bool RecreateSwapChain();

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
