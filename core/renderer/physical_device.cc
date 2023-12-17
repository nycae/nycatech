//
// Created by rplaz on 2023-12-17.
//

#include "physical_device.h"

namespace NycaTech::Renderer {

Vector<PhysicalDevice*> PhysicalDevice::physicalDevices;

Vector<Uint32> PhysicalDevice::GraphicsQueueIndices() const
{
  Vector<Uint32> indices;
  for (Uint32 i = 0; i < familyProperties.Count(); i++) {
    const auto properties = familyProperties[i];
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.Insert(i);
    }
  }
  return indices;
}

Vector<Uint32> PhysicalDevice::PresentationQueueIndices(const Surface* surface) const
{
  Vector<Uint32> indices;
  for (Uint32 i = 0; i < familyProperties.Count(); i++) {
    VkBool32 canPresent;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->surface, &canPresent);
    if (canPresent) {
      indices.Insert(i);
    }
  }
  return indices;
}

Vector<Uint32> PhysicalDevice::CompleteQueueIndices(const Surface* surface) const
{
  Vector<Uint32> indices;
  for (Uint32 i = 0; i < familyProperties.Count(); i++) {
    VkBool32 canPresent;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->surface, &canPresent);
    const auto canDoGraphics = familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
    if (canDoGraphics && canPresent) {
      indices.Insert(i);
    }
  }
  return indices;
}

Vector<PhysicalDevice*> PhysicalDevice::SuitableDevices(VulkanInstance* instance)
{
  Vector<PhysicalDevice*> devices;
  for (const auto& device : GetAllPhysicalDevices(instance)) {
    if (device->HasValidExtensions(instance->Extensions)) {
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
    for (const auto& vkDevice : vkDevices) {
      auto device = new PhysicalDevice();
      device->device = vkDevice;

      vkGetPhysicalDeviceQueueFamilyProperties(device->device, &device->familyProperties.CountMut(), nullptr);
      device->familyProperties.AdjustSize();
      vkGetPhysicalDeviceQueueFamilyProperties(
          device->device, &device->familyProperties.CountMut(), device->familyProperties.Data());
      physicalDevices.Insert(device);
    }
  }
  return physicalDevices;
}

bool PhysicalDevice::HasValidExtensions(const Vector<const char*>& extensions)
{
  Vector<VkExtensionProperties> deviceExtensions;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensions.CountMut(), nullptr);
  deviceExtensions.AdjustSize();
  vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensions.CountMut(), deviceExtensions.Data());

  for (const auto& extension : extensions) {
    auto compare
        = [&extension](const auto& deviceExtension) { return strcmp(extension, deviceExtension.extensionName) == 0; };
    if (deviceExtensions.Contains(compare)) {
      return true;
    }
  }
  return false;
}

}  // namespace NycaTech::Renderer