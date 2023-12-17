//
// Created by rplaz on 2023-12-17.
//

#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include "lib/vector.h"
#include "types.h"
#include "vulkan_instance.h"

namespace NycaTech::Renderer {

class PhysicalDevice final {
public:
  ~PhysicalDevice();
   PhysicalDevice(PhysicalDevice&&) = delete;
   PhysicalDevice(const PhysicalDevice&) = delete;

  PhysicalDevice&&      operator=(PhysicalDevice&&) = delete;
  const PhysicalDevice& operator=(const PhysicalDevice&) = delete;

private:
  PhysicalDevice();

public:
  static Vector<PhysicalDevice*> DevicesWithExtensions(VulkanInstance* instance, const Vector<const char*>& extensions);

private:
  static const Vector<PhysicalDevice*>& GetAllPhysicalDevices(VulkanInstance* instance);
  static Vector<PhysicalDevice*>        physicalDevices;

private:
  bool HasValidExtensions(const Vector<const char*>& extensions);

public:
  VkPhysicalDevice device;
};

}  // namespace NycaTech::Renderer

#endif  // PHYSICAL_DEVICE_H
