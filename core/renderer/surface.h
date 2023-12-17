//
// Created by rplaz on 2023-12-17.
//

#ifndef SURFACE_H
#define SURFACE_H
#include <SDL_video.h>
#include <vulkan/vulkan_core.h>

#include "vulkan_instance.h"

namespace NycaTech::Renderer {

class Surface {
public:
  static Surface* Create(const VulkanInstance* instance, SDL_Window* window);

private:
  Surface() = default;

public:
  VkSurfaceKHR surface;
};

}  // namespace NycaTech::Renderer

#endif  // SURFACE_H
