//
// Created by rplaz on 2023-12-17.
//

#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H
#include <vulkan/vulkan_core.h>

namespace NycaTech::Renderer {

class Device;
class Surface;
class PhysicalDevice;

class SwapChain {
public:
  static SwapChain* Create(const PhysicalDevice* physicalDevice, const Surface* surface, const Device* device);

private:
  SwapChain() = default;

public:
  VkSwapchainKHR     swapchain;
  VkSurfaceFormatKHR format;
  VkExtent2D         extent;
};

}  // namespace NycaTech::Renderer

#endif  // SWAP_CHAIN_H
