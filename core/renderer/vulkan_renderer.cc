//
// Created by rplaz on 2023-12-14.
//

#include "vulkan_renderer.h"

#include <SDL2/SDL_vulkan.h>
#include <lib/assert.h>

namespace NycaTech::Renderer {

const Vector extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef DEBUG
const Vector layers{ "VK_LAYER_KHRONOS_validation" };
#endif

VulkanRenderer::VulkanRenderer()
{
  Assert(SetupWindow(), "unable to setup widow");
  Assert(CreateInstance(), "unable to create vulkan instance");
  Assert(CreateSurface(), "unable to init surface");
  Assert(CreatePhysicalDevice(), "unable to create physical device");
  Assert(CreateLogicalDevice(), "unable to create logical device");
  Assert(CreateSwapChain(), "unable to create swapchain");
  Assert(CreateImageViews(), "uable to create image views");

  // Assert(CreateRenderPass(), "unable to create render pass");
  // Assert(CreateCommandPool(), "unable to create command pool");
}

VulkanRenderer::~VulkanRenderer()
{
  for (auto& model : models) {
    vkFreeMemory(device, model->indexMemory, nullptr);
    vkFreeMemory(device, model->vertexMemory, nullptr);
    vkDestroyBuffer(device, model->vertexBuffer, nullptr);
    vkDestroyBuffer(device, model->indexBuffer, nullptr);
  }
  for (auto& shader : vertexShaders) {
    vkDestroyShaderModule(device, shader, nullptr);
  }
  for (auto& shader : fragmentShaders) {
    vkDestroyShaderModule(device, shader, nullptr);
  }
  for (const auto& image : images) {
    vkDestroyImage(device, image, nullptr);
  }
  for (const auto& imageView : imageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(device, swapchain, nullptr);
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
  SDL_DestroyWindow(window);
}

bool VulkanRenderer::SetupWindow()
{
  AssertReturnFalse(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "unable to init sdl");
  window = SDL_CreateWindow("NycaTech", 200, 200, 1600, 900, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
  AssertReturnFalse(window, "unable to create window");
  return true;
}

bool VulkanRenderer::CreateInstance()
{
  Uint32 eCount;
  AssertReturnFalse(SDL_Vulkan_GetInstanceExtensions(window, &eCount, nullptr), "unable to load sdl vulkan");

  Vector<const char*> names(eCount);
  AssertReturnFalse(SDL_Vulkan_GetInstanceExtensions(window, &eCount, names.Data()), "list instance extensions");

  VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
  appInfo.pApplicationName = "NycaTech Demo";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 2, 0);
  appInfo.pEngineName = "NycaTech";
  appInfo.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
#ifdef DEBUG
  createInfo.enabledLayerCount = VulkanRenderer::Layers.Count();
  createInfo.ppEnabledLayerNames = VulkanRenderer::Layers.Data();
#else
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;
#endif
  createInfo.enabledExtensionCount = names.Count();
  createInfo.ppEnabledExtensionNames = names.Data();

  AssertReturnFalse(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS,
                    "unable to create vulkan instance");
  return true;
}

bool VulkanRenderer::CreateSurface()
{
  return SDL_Vulkan_CreateSurface(window, instance, &surface) == SDL_TRUE;
}

bool VulkanRenderer::CreatePhysicalDevice()
{
  Uint32 pdCount;
  vkEnumeratePhysicalDevices(instance, &pdCount, nullptr);
  Vector<VkPhysicalDevice> vkDevices(pdCount);
  vkEnumeratePhysicalDevices(instance, &pdCount, vkDevices.Data());

  Vector<VkPhysicalDevice> devices;
  for (const auto& vkDevice : vkDevices) {
    Vector<VkQueueFamilyProperties> properties;
    vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &properties.CountMut(), nullptr);
    properties.AdjustSize();
    vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &properties.CountMut(), properties.Data());
    if (IsDeviceSuitable(vkDevice)) {
      devices.Insert(vkDevice);
    }
  }
  AssertReturnFalse(devices.Count() > 0, "no valid devices found");
  physicalDevice = devices[0];
  return true;
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice vkDevice)
{
  Vector<VkExtensionProperties> deviceExtensions;
  vkEnumerateDeviceExtensionProperties(vkDevice, nullptr, &deviceExtensions.CountMut(), nullptr);
  deviceExtensions.AdjustSize();
  vkEnumerateDeviceExtensionProperties(vkDevice, nullptr, &deviceExtensions.CountMut(), deviceExtensions.Data());

  for (const auto& extension : extensions) {
    if (deviceExtensions.Contains([&extension](const auto& deviceExtension) {
          return strcmp(extension, deviceExtension.extensionName) == 0;
        })) {
      return true;
    }
  }
  return false;
}

Vector<Uint32> VulkanRenderer::PresentationQueueIndices() const
{
  Vector<VkQueueFamilyProperties> familyProperties;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyProperties.CountMut(), nullptr);
  familyProperties.AdjustSize();
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyProperties.CountMut(), familyProperties.Data());

  Vector<Uint32> indices;
  for (Uint32 i = 0; i < familyProperties.Count(); i++) {
    VkBool32 canPresent;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &canPresent);
    if (canPresent) {
      indices.Insert(i);
    }
  }
  return indices;
}

Vector<Uint32> VulkanRenderer::GraphicsQueueIndices() const
{
  Vector<VkQueueFamilyProperties> familyProperties;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyProperties.CountMut(), nullptr);
  familyProperties.AdjustSize();
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyProperties.CountMut(), familyProperties.Data());

  Vector<Uint32> indices;
  for (Uint32 i = 0; i < familyProperties.Count(); i++) {
    const auto properties = familyProperties[i];
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.Insert(i);
    }
  }
  return indices;
}

bool VulkanRenderer::CreateLogicalDevice()
{
  graphicsQueueIndex = GraphicsQueueIndices()[0];
  presentQueueIndex = PresentationQueueIndices()[0];

  VkPhysicalDeviceFeatures        dFeatures{ .fillModeNonSolid = VK_TRUE };
  Float32                         queuePriority = 1.0f;
  Vector<VkDeviceQueueCreateInfo> infos;

  VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
  queueInfo.queueFamilyIndex = graphicsQueueIndex, queueInfo.queueCount = 1;
  queueInfo.pQueuePriorities = &queuePriority;

  infos.Insert(queueInfo);

  if (graphicsQueueIndex != presentQueueIndex) {
    VkDeviceQueueCreateInfo queue2Info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queue2Info.queueFamilyIndex = presentQueueIndex;
    queue2Info.queueCount = 1;
    queue2Info.pQueuePriorities = &queuePriority;
    infos.Insert(queue2Info);
  }

  VkDeviceCreateInfo dcInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
  dcInfo.queueCreateInfoCount = infos.Count();
  dcInfo.pQueueCreateInfos = infos.Data();
  dcInfo.enabledLayerCount = 0;
  dcInfo.ppEnabledLayerNames = nullptr;
  dcInfo.enabledExtensionCount = VulkanRenderer::Extensions.Count();
  dcInfo.ppEnabledExtensionNames = VulkanRenderer::Extensions.Data();
  dcInfo.pEnabledFeatures = &dFeatures;

  AssertVKReturnFalse(vkCreateDevice(physicalDevice, &dcInfo, nullptr, &device), "unable to crate device");
  vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
  AssertReturnFalse(graphicsQueue, "unable to fetch graphics queue");
  vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);
  AssertReturnFalse(presentQueue, "unable to fetch present queue");
  return true;
}

bool VulkanRenderer::CreateSwapChain()
{
  Vector<VkSurfaceFormatKHR> formats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formats.CountMut(), nullptr);
  formats.AdjustSize();
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formats.CountMut(), formats.Data());

  Vector<VkPresentModeKHR> modes;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modes.CountMut(), nullptr);
  modes.AdjustSize();
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modes.CountMut(), modes.Data());

  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

  format = ChooseFormat(formats);
  extent = capabilities.currentExtent.width != UINT32_MAX ? capabilities.currentExtent : VkExtent2D{ 1600, 900 };

  const Uint32 indices[] = { presentQueueIndex, graphicsQueueIndex };

  VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  info.surface = surface;
  info.minImageCount = capabilities.minImageCount + 1;
  info.imageFormat = format.format;
  info.imageColorSpace = format.colorSpace;
  info.imageExtent = extent;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.imageSharingMode
      = (presentQueueIndex == graphicsQueueIndex) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
  info.queueFamilyIndexCount = (presentQueueIndex == graphicsQueueIndex) ? 0u : 2u;
  info.pQueueFamilyIndices = (presentQueueIndex == graphicsQueueIndex) ? nullptr : indices;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.presentMode = ChooseMode(modes);
  info.clipped = true;
  info.oldSwapchain = nullptr;
  AssertVKReturnFalse(vkCreateSwapchainKHR(device, &info, nullptr, &swapchain), "unable to create swapchain");
  vkGetSwapchainImagesKHR(device, swapchain, &images.CountMut(), nullptr);
  images.AdjustSize();
  vkGetSwapchainImagesKHR(device, swapchain, &images.CountMut(), images.Data());
  return true;
}

VkSurfaceFormatKHR VulkanRenderer::ChooseFormat(const Vector<VkSurfaceFormatKHR>& formats)
{
  for (const auto& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return formats[0];
}

VkPresentModeKHR VulkanRenderer::ChooseMode(const Vector<VkPresentModeKHR>& modes)
{
  for (const auto& mode : modes) {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

bool VulkanRenderer::CreateImageViews()
{
  imageViews.CountMut() = images.Count();
  imageViews.AdjustSize();
  for (Uint32 i = 0; i < images.Count(); i++) {
    VkImageViewCreateInfo info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    info.image = images[i];
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format.format;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    AssertVKReturnFalse(vkCreateImageView(device, &info, nullptr, &imageViews[i]), "unable to load images views");
  }
  return true;
}

bool VulkanRenderer::AttachShader(const Shader& shader)
{
  VkShaderModule           module;
  VkShaderModuleCreateInfo info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
  info.codeSize = shader.length;
  info.pCode = (Uint32*)shader.content;

  AssertVKReturnFalse(vkCreateShaderModule(device, &info, nullptr, &module), "unable to load shader");

  switch (shader.type) {
    case Shader::Type::VERTEX:
      return vertexShaders.Insert(module);
    case Shader::Type::FRAGMENT:
      return fragmentShaders.Insert(module);
    default:
      return false;
  }
}

bool VulkanRenderer::CreateRenderPipeline()
{
  Vector<VkPipelineShaderStageCreateInfo> shaderStages;
  for (const auto& shader : vertexShaders) {
    VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    info.module = shader;
    info.pName = "main";
    info.pSpecializationInfo = nullptr;
    shaderStages.Insert(info);
  }

  for (const auto& shader : fragmentShaders) {
    VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.module = shader;
    info.pName = "main";
    info.pSpecializationInfo = nullptr;
    shaderStages.Insert(info);
  }

  auto modelBinding = ObjModel::GetVkVertexInputBindingDescription();
  auto attributeBinding = ObjModel::GetVkVertexInputAttributeDescription();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &modelBinding;
  vertexInputInfo.vertexAttributeDescriptionCount = 1;
  vertexInputInfo.pVertexAttributeDescriptions = &attributeBinding;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.sampleShadingEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.colorWriteMask
      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkDynamicState                   dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
  VkPipelineDynamicStateCreateInfo dynamicState{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
  dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState);
  dynamicState.pDynamicStates = dynamicStates;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    return false;
  }

  VkGraphicsPipelineCreateInfo info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
  info.stageCount = shaderStages.Count();
  info.pStages = shaderStages.Data();
  info.pVertexInputState = &vertexInputInfo;
  info.pInputAssemblyState = &inputAssembly;
  info.pTessellationState = nullptr;
  info.pViewportState = &viewportState;
  info.pRasterizationState = &rasterizer;
  info.pMultisampleState = &multisampling;
  info.pDepthStencilState = nullptr;
  info.pColorBlendState = &colorBlending;
  info.pDynamicState = &dynamicState;
  info.layout = pipelineLayout;
  info.renderPass = renderPass;
  info.subpass = 0;
  info.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
    return false;
  }

  return true;
}

bool VulkanRenderer::DrawFrame()
{
  vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  Uint32 imageIndex;

  switch (vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageMutex, VK_NULL_HANDLE, &imageIndex)) {
    case VK_SUBOPTIMAL_KHR:
      RecreateSwapChain();
      return true;
    case VK_SUCCESS:
      break;
    default:
      return false;
  }

  vkResetFences(device, 1, &inFlightFence);
  vkResetCommandBuffer(command, 0);
  if (!RecordCommandBuffer(command, imageIndex)) {
    return false;
  }

  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  VkSubmitInfo         info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &imageMutex;
  info.pWaitDstStageMask = waitStages;
  info.commandBufferCount = 1;
  info.pCommandBuffers = &command;
  info.signalSemaphoreCount = 1;
  info.pSignalSemaphores = &renderMutex;

  if (vkQueueSubmit(graphicsQueue, 1, &info, inFlightFence) != VK_SUCCESS) {
    return false;
  }

  VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderMutex;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain;
  presentInfo.pImageIndices = &imageIndex;

  switch (vkQueuePresentKHR(presentQueue, &presentInfo)) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
      // swapchain->Rebuild(physicalDevice, device);
    case VK_SUCCESS:
      return true;
    default:
      return false;
  }
}

bool VulkanRenderer::CreateFrameBuffers()
{
  //   frameBuffer.Resize(swapchain->imageViews.Count());
  //   frameBuffer.OverrideCount(swapchain->imageViews.Count());
  //   for (Uint32 i = 0; i < swapchain->imageViews.Count(); i++) {
  //     VkFramebufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
  //     bufferInfo.renderPass = renderPass;
  //     bufferInfo.attachmentCount = 1;
  //     bufferInfo.pAttachments = &swapchain->imageViews[i];
  //     bufferInfo.width = swapchain->extent.width;
  //     bufferInfo.height = swapchain->extent.height;
  // #ifdef DEBUG
  //     bufferInfo.layers = VulkanRenderer::Layers.Count();
  // #endif
  //
  //     if (vkCreateFramebuffer(device, &bufferInfo, nullptr, &frameBuffer[i]) != VK_SUCCESS) {
  //       return false;
  //     }
  //   }
  //   return true;
  return false;
}

bool VulkanRenderer::CreateSynch()
{
  VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  VkFenceCreateInfo     fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };

  return vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) == VK_SUCCESS
         && vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageMutex) == VK_SUCCESS
         && vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderMutex) == VK_SUCCESS;
}

bool VulkanRenderer::CreateCommandPool()
{
  VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = graphicsQueueIndex;

  return vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) == VK_SUCCESS;
}

bool VulkanRenderer::CreateCommandBuffers()
{
  VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  return vkAllocateCommandBuffers(device, &allocInfo, &command) == VK_SUCCESS;
}

bool VulkanRenderer::RecreateSwapChain()
{
  // return vkDeviceWaitIdle(device) && swapchain->Rebuild(physicalDevice, device) && swapchain->LoadImageViews(device)
  //        && CreateFrameBuffers();
  return false;
}

bool VulkanRenderer::CreateUniformBuffers()
{
  // const VkDeviceSize bufferSize = sizeof(Uniform);
  // for (Uint32 i = 0; i < swapchain->images.Count(); i++) {
  //   const auto buffer = CreateBuffer(bufferSize,
  //                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
  //                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //                                    uniformBuffersMemory[i]);
  //   AssertReturnFalse(buffer, "unable to create uniform buffer");
  //   uniformBuffers[i] = buffer;
  //   vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
  // }
  // return true;
  return false;
}

bool VulkanRenderer::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
  VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  VkBufferCopy copy{ 0, 0, size };
  vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copy);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    return false;
  }

  VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    return false;
  }

  if (vkQueueWaitIdle(graphicsQueue) != VK_SUCCESS) {
    return false;
  }

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

  return true;
}

VkBuffer VulkanRenderer::CreateBuffer(VkDeviceSize          size,
                                      VkBufferUsageFlags    usage,
                                      VkMemoryPropertyFlags properties,
                                      VkDeviceMemory&       bufferMemory)
{
  VkBuffer           buffer;
  VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    return nullptr;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  Uint32 index = 0;
  for (Uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
    const bool isMemoryBits = memRequirements.memoryTypeBits & (1 << i);
    const bool hasRequiredProperties = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;
    if (isMemoryBits && hasRequiredProperties) {
      index = i;
      break;
    }
  }
  AssertReturnNull(index, "unable to finde suitable memory");

  VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = index;

  AssertReturnNull(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS,
                   "unable to allocate memory");
  AssertReturnNull(vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS, "unable to bind buffer");

  return buffer;
}

bool VulkanRenderer::CreateVertexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data)
{
  VkDeviceMemory bufferMemory;
  VkBuffer       stageBuffer = CreateBuffer(size,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      bufferMemory);
  AssertReturnFalse(stageBuffer, "unable to create stage buffer");
  void* dst;
  vkMapMemory(device, bufferMemory, 0, size, 0, &dst);
  memcpy(dst, data, size);
  vkUnmapMemory(device, bufferMemory);

  buffer = CreateBuffer(size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        memory);
  AssertReturnFalse(buffer, "unable to create buffer");
  AssertReturnFalse(CopyBuffer(stageBuffer, buffer, size), "unable to copy the buffer");
  vkDestroyBuffer(device, stageBuffer, nullptr);
  vkFreeMemory(device, bufferMemory, nullptr);
  return true;
}

bool VulkanRenderer::CreateIndexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data)
{
  VkDeviceMemory bufferMemory;
  VkBuffer       stageBuffer = CreateBuffer(size,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      bufferMemory);
  AssertReturnFalse(stageBuffer, "unable to create stage buffer");
  void* dst;
  vkMapMemory(device, bufferMemory, 0, size, 0, &dst);
  memcpy(dst, data, size);
  vkUnmapMemory(device, bufferMemory);

  buffer = CreateBuffer(size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        memory);
  AssertReturnFalse(buffer, "unable to create buffer");
  AssertReturnFalse(!CopyBuffer(stageBuffer, buffer, size), "unable to copy buffer");
  vkDestroyBuffer(device, stageBuffer, nullptr);
  vkFreeMemory(device, bufferMemory, nullptr);
  return true;
}

bool VulkanRenderer::RecordCommandBuffer(VkCommandBuffer command, Uint32 imageIndex)
{
  VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS) {
    return false;
  }

  VkClearValue          clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
  VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = frameBuffer[imageIndex];
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = extent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkViewport viewport{ 0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.f, 1.f };
  vkCmdSetViewport(command, 0, 1, &viewport);

  VkRect2D scissor{ { 0, 0 }, extent };
  vkCmdSetScissor(command, 0, 1, &scissor);

  VkDeviceSize offsets[] = { 0 };
  for (const auto& model : models) {
    vkCmdBindVertexBuffers(command, 0, 1, &model->vertexBuffer, offsets);
    vkCmdBindIndexBuffer(command, model->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command, model->indices.Count(), 1, 0, 0, 0);
  }

  vkCmdEndRenderPass(command);

  return vkEndCommandBuffer(command) == VK_SUCCESS;
}

bool VulkanRenderer::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  // colorAttachment.format = swapchain->format.format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  return vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) == VK_SUCCESS;
}

bool VulkanRenderer::LoadModel(ObjModel* model)
{
  const auto& vertices = model->vertices;
  const auto  vertSize = sizeof(vertices[0]) * vertices.Count();
  if (!CreateVertexBuffer(model->vertexBuffer, model->vertexMemory, vertSize, (void*)vertices.Data())) {
    return false;
  }

  const auto& indices = model->indices;
  const auto  indexSize = sizeof(indices[0]) * indices.Count();
  if (!CreateIndexBuffer(model->indexBuffer, model->indexMemory, indexSize, (void*)indices.Data())) {
    return false;
  }
  return models.Insert(model);
}

}  // namespace NycaTech::Renderer