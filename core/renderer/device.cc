//
// Created by rplaz on 2023-12-17.
//

#include "device.h"

#include "lib/assert.h"
#include "physical_device.h"
#include "vulkan_queue.h"

namespace NycaTech::Renderer {

Device::~Device()
{
  vkDestroyDevice(device, nullptr);
}

Device* Device::Create(PhysicalDevice* physicalDevice, const Surface* surface)
{
  Device* device = new Device();
  device->graphicsQueue = new VulkanQueue();
  device->presentQueue = new VulkanQueue();

  device->graphicsQueue->index = physicalDevice->GraphicsQueueIndices()[0];
  device->presentQueue->index = physicalDevice->PresentationQueueIndices(surface)[0];

  VkPhysicalDeviceFeatures        dFeatures{ .fillModeNonSolid = VK_TRUE };
  Float32                         queuePriority = 1.0f;
  Vector<VkDeviceQueueCreateInfo> infos;

  VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
  queueInfo.queueFamilyIndex = device->graphicsQueue->index, queueInfo.queueCount = 1;
  queueInfo.pQueuePriorities = &queuePriority;

  infos.Insert(queueInfo);

  if (device->graphicsQueue->index != device->presentQueue->index) {
    VkDeviceQueueCreateInfo queue2Info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queue2Info.queueFamilyIndex = device->presentQueue->index;
    queue2Info.queueCount = 1;
    queue2Info.pQueuePriorities = &queuePriority;
    infos.Insert(queue2Info);
  }

  VkDeviceCreateInfo dcInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  dcInfo.queueCreateInfoCount = infos.Count();
  dcInfo.pQueueCreateInfos = infos.Data();
  dcInfo.enabledLayerCount = 0;
  dcInfo.ppEnabledLayerNames = nullptr;
  dcInfo.enabledExtensionCount = VulkanInstance::Extensions.Count();
  dcInfo.ppEnabledExtensionNames = VulkanInstance::Extensions.Data();
  dcInfo.pEnabledFeatures = &dFeatures;


  AssertReturnNull(vkCreateDevice(physicalDevice->device, &dcInfo, nullptr, &device->device) == VK_SUCCESS,
                   "unable to crate device");
  vkGetDeviceQueue(device->device, device->graphicsQueue->index, 0, &device->graphicsQueue->queue);
  AssertReturnNull(device->graphicsQueue->queue, "unable to fetch graphics queue");
  vkGetDeviceQueue(device->device, device->presentQueue->index, 0, &device->presentQueue->queue);
  AssertReturnNull(device->presentQueue->queue, "unable to fetch present queue");
  return device;
}

};  // namespace NycaTech::Renderer