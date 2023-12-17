//
// Created by rplaz on 2023-12-14.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "device.h"
#include "lib/types.h"
#include "obj_model.h"
#include "physical_device.h"
#include "shader.h"
#include "swapchain.h"

namespace NycaTech::Renderer {

class VulkanRenderer final {
public:
       VulkanRenderer(SDL_Window* window);
  bool AttachShader(Shader* shader);
  bool LoadModel(ObjModel* model);
  bool PrepareRendering();
  bool DrawFrame();

private:
  VulkanRenderer() = default;

public:
  static VulkanRenderer* Create(PhysicalDevice* physicalDevice, SwapChain* swapchain, Device* device);

public:
  PhysicalDevice*       physicalDevice;
  Device*               device;
  SwapChain*            swapchain;
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

private:
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
