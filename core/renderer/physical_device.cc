//
// Created by rplaz on 2023-12-17.
//

#include "physical_device.h"

namespace NycaTech::Renderer {

PhysicalDevice::~PhysicalDevice()
{
}

PhysicalDevice::PhysicalDevice()
{
}

Vector<PhysicalDevice*> PhysicalDevice::DevicesWithExtensions(VulkanInstance* instance,
                                                              const Vector<const char*>& extensions)
{
  Vector<PhysicalDevice*> devices;
  for (const auto& device: GetAllPhysicalDevices(instance)) {
    if (device->HasValidExtensions(extensions)) {
      devices.Insert(device);
    }
  }
  return devices;
}

const Vector<PhysicalDevice*>& PhysicalDevice::GetAllPhysicalDevices(VulkanInstance* instance)
{
  if (physicalDevices.Count() <= 0) {
    Uint32 pdCount;
    vkEnumeratePhysicalDevices(instance->instance, &pdCount, nullptr);
    Vector<VkPhysicalDevice> vkDevices(pdCount);
    vkEnumeratePhysicalDevices(instance->instance, &pdCount, vkDevices.Data());
    for (const auto& vkDevice: vkDevices) {
      auto device = new PhysicalDevice();
      device->device = vkDevice;
      physicalDevices.Insert(device);
    }
  }
  return physicalDevices;
}

bool PhysicalDevice::HasValidExtensions(const Vector<const char*>& extensions)
{
  Uint32 extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  Vector<VkExtensionProperties> deviceExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.Data());

  auto comparate = [](const char*& extensionName) {
    return [&extensionName](const VkExtensionProperties& other) {
      return strcmp(other.extensionName, extensionName) == 0;
    };
  };

  for (auto extension : extensions) {
    if (!deviceExtensions.Contains(comparate(extension))) {
      return false;
    }
  }
  return true;
}

}  // namespace NycaTech::Renderer