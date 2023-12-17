//
// Created by rplaz on 2023-12-17.
//

#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H
#include <SDL_video.h>
#include <vulkan/vulkan.h>

namespace NycaTech::Renderer {

class VulkanInstance final {
public:
  ~                      VulkanInstance();
  static VulkanInstance* Create(SDL_Window* window);

private:
  VulkanInstance() = default;

public:
  VkInstance instance;
};

};  // namespace NycaTech::Renderer

#endif  // VULKANINSTANCE_H
