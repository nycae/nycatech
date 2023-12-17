//
// Created by rplaz on 2023-12-17.
//

#ifndef SWAP_CHAIN_H
#define SWAP_CHAIN_H
#include <lib/vector.h>
#include <vulkan/vulkan_core.h>

namespace NycaTech::Renderer {

class Device;
class Surface;
class PhysicalDevice;

class SwapChain {
public:
  static SwapChain* Create(const PhysicalDevice* physicalDevice, const Surface* surface, const Device* device);
  bool              LoadImages(const Device* device);
  bool              LoadImageViews(const Device* device);
  bool              Rebuild(const PhysicalDevice* physicalDevice, const Device* device);

private:
  SwapChain() = default;

public:
  const Surface* surface;

public:
  VkSwapchainKHR      swapchain;
  VkSurfaceFormatKHR  format;
  VkExtent2D          extent;
  Vector<VkImage>     images;
  Vector<VkImageView> imageViews;
};

}  // namespace NycaTech::Renderer

#endif  // SWAP_CHAIN_H
