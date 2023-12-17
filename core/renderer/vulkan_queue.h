//
// Created by rplaz on 2023-12-17.
//

#ifndef VULKANQUEUE_H
#define VULKANQUEUE_H
#include <vulkan/vulkan_core.h>

#include "lib/types.h"

namespace NycaTech::Renderer {

class Device;

class VulkanQueue {
public:
  friend class Device;

  ~VulkanQueue() = default;

private:
  VulkanQueue() = default;

public:
  VkQueue queue;
  Uint32  index;
};

}  // namespace NycaTech::Renderer

#endif  // VULKANQUEUE_H
