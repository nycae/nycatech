//
// Created by rplaz on 2023-12-17.
//

#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include "../lib/types.h"
#include "lib/vector.h"
#include "surface.h"
#include "vulkan_instance.h"

namespace NycaTech::Renderer {

class PhysicalDevice final {
public:
  ~PhysicalDevice() = default;
   PhysicalDevice(PhysicalDevice&&) = delete;
   PhysicalDevice(const PhysicalDevice&) = delete;

  PhysicalDevice&&      operator=(PhysicalDevice&&) = delete;
  const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

public:
  Vector<Uint32> GraphicsQueueIndices() const;
  Vector<Uint32> PresentationQueueIndices(const Surface* surface) const;
  Vector<Uint32> CompleteQueueIndices(const Surface* surface) const;

private:
  PhysicalDevice() = default;

public:
  static Vector<PhysicalDevice*> SuitableDevices(VulkanInstance* instance);

private:
  static const Vector<PhysicalDevice*>& GetAllPhysicalDevices(VulkanInstance* instance);
  static Vector<PhysicalDevice*>        physicalDevices;

private:
  bool HasValidExtensions(const Vector<const char*>& extensions);

public:
  VkPhysicalDevice                device;
  Vector<VkQueueFamilyProperties> familyProperties;

};

}  // namespace NycaTech::Renderer

#endif  // PHYSICAL_DEVICE_H
