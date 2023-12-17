//
// Created by rplaz on 2023-12-17.
//

#include "vulkan_instance.h"

#include <SDL_video.h>
#include <SDL_vulkan.h>

#include "lib/assert.h"
#include "lib/vector.h"

namespace NycaTech::Renderer {

#ifdef DEBUG
constexpr const char* layers[] = {
  "VK_LAYER_KHRONOS_validation",
};
#endif

VulkanInstance::~VulkanInstance()
{
  vkDestroyInstance(instance, nullptr);
}

VulkanInstance* VulkanInstance::Create(SDL_Window* window)
{
  Uint32 eCount;
  AssertReturnNull(SDL_Vulkan_GetInstanceExtensions(window, &eCount, nullptr), "unable to load sdl vulkan");

  Vector<const char*> names(eCount);
  AssertReturnNull(SDL_Vulkan_GetInstanceExtensions(window, &eCount, names.Data()), "list instance extensions");

  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.pApplicationName = "NycaTech Demo";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 2, 0);
  appInfo.pEngineName = "NycaTech";
  appInfo.apiVersion = VK_API_VERSION_1_3;
  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
#ifdef DEBUG
  createInfo.enabledLayerCount = sizeof(layers) / sizeof(const char*);
  createInfo.ppEnabledLayerNames = layers;
#else
  .enabledLayerCount = 0 .ppEnabledLayerNames = nullptr,
#endif
  createInfo.enabledExtensionCount = names.Count();
  createInfo.ppEnabledExtensionNames = names.Data();

  VulkanInstance* instance = new VulkanInstance();
  AssertReturnNull(vkCreateInstance(&createInfo, nullptr, &instance->instance) == VK_SUCCESS,
                   "unable to create vulkan instance");

  return instance;
}


};  // namespace NycaTech::Renderer