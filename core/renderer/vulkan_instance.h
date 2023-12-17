//
// Created by rplaz on 2023-12-17.
//

#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H
#include <SDL_video.h>
#include <vulkan/vulkan.h>

#include "lib/vector.h"

namespace NycaTech::Renderer {

class VulkanInstance final {
public:
  ~                      VulkanInstance();
  static VulkanInstance* Create(SDL_Window* window);

public:
  inline static const Vector Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef DEBUG
  inline static const Vector Layers{ "VK_LAYER_KHRONOS_validation" };
#endif

private:
  VulkanInstance() = default;

public:
  VkInstance instance;
};

};  // namespace NycaTech::Renderer

#endif  // VULKANINSTANCE_H
