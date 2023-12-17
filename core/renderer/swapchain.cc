//
// Created by rplaz on 2023-12-17.
//

#include "swapchain.h"

#include <lib/assert.h>
#include <lib/vector.h>

#include "device.h"
#include "physical_device.h"
#include "surface.h"
#include "vulkan_queue.h"

namespace NycaTech::Renderer {

VkSurfaceFormatKHR ChooseFormat(const Vector<VkSurfaceFormatKHR>& formats)
{
  for (const auto& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return formats[0];
};

VkPresentModeKHR ChooseMode(const Vector<VkPresentModeKHR>& modes)
{
  for (const auto& mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
};

VkExtent2D GetExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }
  return VkExtent2D{ 1600, 900 };
};

SwapChain* SwapChain::Create(const PhysicalDevice* physicalDevice, const Surface* surface, const Device* device)
{
  SwapChain* swapchain = new SwapChain();
  swapchain->surface = surface;
  AssertReturnNull(swapchain->Rebuild(physicalDevice, device), "unable to build swapchain");
  AssertReturnNull(swapchain->LoadImages(device), "unable to load images");
  AssertReturnNull(swapchain->LoadImageViews(device), "unable to load image views");
  return swapchain;
}

bool SwapChain::LoadImages(const Device* device)
{
  AssertReturnFalse(vkGetSwapchainImagesKHR(device->device, swapchain, &images.CountMut(), nullptr) == VK_SUCCESS,
                    "unable to create images");
  images.AdjustSize();

  AssertReturnFalse(vkGetSwapchainImagesKHR(device->device, swapchain, &images.CountMut(), images.Data()) == VK_SUCCESS,
                    "unable to create images");
  return true;
}

bool SwapChain::LoadImageViews(const Device* device)
{
  imageViews.Resize(images.Count());
  imageViews.OverrideCount(images.Count());
  for (Uint32 i = 0; i < images.Count(); i++) {
    VkImageViewCreateInfo info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    info.image = images[i];
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format.format;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    AssertReturnFalse(vkCreateImageView(device->device, &info, nullptr, &imageViews[i]) == VK_SUCCESS,
                      "unable to load images views");
  }
  return true;
}

bool SwapChain::Rebuild(const PhysicalDevice* physicalDevice, const Device* device)
{
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

  Self.format = ChooseFormat(formats);
  Self.extent = GetExtent(capabilities);

  Uint32 indices[] = { device->presentQueue->index, device->graphicsQueue->index };

  VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  info.surface = surface->surface;
  info.minImageCount = capabilities.minImageCount + 1;
  info.imageFormat = format.format;
  info.imageColorSpace = format.colorSpace;
  info.imageExtent = extent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.imageSharingMode = (device->presentQueue->index == device->graphicsQueue->index) ? VK_SHARING_MODE_EXCLUSIVE
                                                                                        : VK_SHARING_MODE_CONCURRENT;
  info.queueFamilyIndexCount = (device->presentQueue->index == device->graphicsQueue->index) ? 0u : 2u;
  info.pQueueFamilyIndices = (device->presentQueue->index == device->graphicsQueue->index) ? nullptr : indices;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = ChooseMode(modes);
  info.clipped = true;
  info.oldSwapchain = nullptr;
  AssertReturnFalse(vkCreateSwapchainKHR(device->device, &info, nullptr, &swapchain) == VK_SUCCESS,
                    "unable to create swapchain");
  return true;
}

}  // namespace NycaTech::Renderer