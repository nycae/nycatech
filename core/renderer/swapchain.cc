//
// Created by rplaz on 2023-12-17.
//

#include <lib/assert.h>
#include <lib/vector.h>

#include "device.h"
#include "physical_device.h"
#include "surface.h"
#include "swapchain.h"
#include "vulkan_queue.h"

namespace NycaTech::Renderer {

SwapChain* SwapChain::Create(const PhysicalDevice* physicalDevice, const Surface* surface, const Device* device)
{
  SwapChain* swapchain = new SwapChain();
  Vector<VkSurfaceFormatKHR> formats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->device, surface->surface, &formats.CountMut(), nullptr);
  formats.AdjustSize();
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->device, surface->surface, &formats.CountMut(), formats.Data());

  Vector<VkPresentModeKHR> modes;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->device, surface->surface, &modes.CountMut(), nullptr);
  modes.AdjustSize();
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->device, surface->surface, &modes.CountMut(), modes.Data());

  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->device, surface->surface, &capabilities);

  auto swapChainSurfaceFormat = [&formats]() -> VkSurfaceFormatKHR {
    for (const auto& format : formats) {
      if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return format;
      }
    }
    return formats[0];
  };
  auto swapChainPresentMode = [&modes]() -> VkPresentModeKHR {
    for (const auto& mode : modes) {
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return mode;
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
  };
  auto swapChanExtent = [&capabilities]() -> VkExtent2D {
    if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
    }
    return VkExtent2D{ 1600, 900 };
  };

  swapchain->format = swapChainSurfaceFormat();
  swapchain->extent = swapChanExtent();

  Uint32 indices[] = { device->presentQueue->index, device->graphicsQueue->index };

  VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  info.surface = surface->surface;
  info.minImageCount = capabilities.minImageCount + 1;
  info.imageFormat = swapchain->format.format;
  info.imageColorSpace = swapchain->format.colorSpace;
  info.imageExtent = swapchain->extent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.imageSharingMode = (device->presentQueue->index == device->graphicsQueue->index) ? VK_SHARING_MODE_EXCLUSIVE
                                                                                        : VK_SHARING_MODE_CONCURRENT;
  info.queueFamilyIndexCount = (device->presentQueue->index == device->graphicsQueue->index) ? 0u : 2u;
  info.pQueueFamilyIndices = (device->presentQueue->index == device->graphicsQueue->index) ? nullptr : indices;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = swapChainPresentMode();
  info.clipped = true;
  info.oldSwapchain = nullptr;


  AssertReturnNull(vkCreateSwapchainKHR(device->device, &info, nullptr, &swapchain->swapchain) == VK_SUCCESS,
                   "unable to create swapchain");
  return swapchain;
}

}  // namespace NycaTech::Renderer