//
// Created by rplaz on 2023-12-17.
//

#include "surface.h"

#include <SDL_vulkan.h>

#include "lib/assert.h"

namespace NycaTech::Renderer {

Surface* Surface::Create(const VulkanInstance* instance, SDL_Window* window)
{
  auto surf = new Surface();
  AssertReturnNull(SDL_Vulkan_CreateSurface(window, instance->instance, &surf->surface), "unable to init surface");
  return surf;
}

}  // namespace NycaTech::Renderer