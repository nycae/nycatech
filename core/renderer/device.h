//
// Created by rplaz on 2023-12-17.
//

#ifndef DEVICE_H
#define DEVICE_H
#include "physical_device.h"

namespace NycaTech::Renderer {
class PhysicalDevice;
class Surface;
class VulkanQueue;

class Device {
public:
  ~Device();

public:
  static Device* Create(PhysicalDevice* physicalDevice, const Surface* surface);

private:
  Device() = default;

public:
  VkDevice     device;
  VulkanQueue* graphicsQueue;
  VulkanQueue* presentQueue;
};

};  // namespace NycaTech::Renderer

#endif  // DEVICE_H
