//
// Created by rplaz on 2023-12-14.
//

#include "vulkan_renderer.h"

#include <SDL2/SDL_vulkan.h>
#include <lib/assert.h>

#include "device.h"
#include "vulkan_queue.h"

namespace NycaTech::Renderer {

const Vector extensions{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#ifdef DEBUG
const Vector layers{
  "VK_LAYER_KHRONOS_validation",
};
#endif

VulkanRenderer::VulkanRenderer(SDL_Window* window)
{
  Assert(CreateRenderPass(), "unable to create render pass");
  Assert(CreateCommandPool(), "unable to create command pool");
}

bool VulkanRenderer::AttachShader(Shader* shader)
{
  return shaders.Insert(shader);
}

bool VulkanRenderer::CreateRenderPipeline()
{
  Vector<VkPipelineShaderStageCreateInfo> shaderStages;
  for (const auto& shader : shaders) {
    VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    info.stage = shader->type == Shader::Type::VERTEX ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
    info.module = shader->module;
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
  if (vkCreatePipelineLayout(device->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
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

  if (vkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
    return false;
  }

  return true;
}

bool VulkanRenderer::DrawFrame()
{
  vkWaitForFences(device->device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
  Uint32 imageIndex;

  switch (vkAcquireNextImageKHR(
      device->device, swapchain->swapchain, UINT64_MAX, imageMutex, VK_NULL_HANDLE, &imageIndex)) {
    case VK_SUBOPTIMAL_KHR:
      RecreateSwapChain();
      return true;
    case VK_SUCCESS:
      break;
    default:
      return false;
  }

  vkResetFences(device->device, 1, &inFlightFence);
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

  if (vkQueueSubmit(device->graphicsQueue->queue, 1, &info, inFlightFence) != VK_SUCCESS) {
    return false;
  }

  VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderMutex;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain->swapchain;
  presentInfo.pImageIndices = &imageIndex;

  switch (vkQueuePresentKHR(device->presentQueue->queue, &presentInfo)) {
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
      swapchain->Rebuild(physicalDevice, device);
    case VK_SUCCESS:
      return true;
    default:
      return false;
  }
}

VulkanRenderer* VulkanRenderer::Create(PhysicalDevice* physicalDevice, SwapChain* swapchain, Device* device)
{
  auto renderer = new VulkanRenderer();
  renderer->physicalDevice = physicalDevice;
  renderer->swapchain = swapchain;
  renderer->device = device;
  Assert(renderer->CreateRenderPass(), "unable to create render pass");
  Assert(renderer->CreateCommandPool(), "unable to create command pool");
  return renderer;
}

bool VulkanRenderer::CreateFrameBuffers()
{
  frameBuffer.Resize(swapchain->imageViews.Count());
  frameBuffer.OverrideCount(swapchain->imageViews.Count());
  for (Uint32 i = 0; i < swapchain->imageViews.Count(); i++) {
    VkFramebufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    bufferInfo.renderPass = renderPass;
    bufferInfo.attachmentCount = 1;
    bufferInfo.pAttachments = &swapchain->imageViews[i];
    bufferInfo.width = swapchain->extent.width;
    bufferInfo.height = swapchain->extent.height;
#ifdef DEBUG
    bufferInfo.layers = VulkanInstance::Layers.Count();
#endif

    if (vkCreateFramebuffer(device->device, &bufferInfo, nullptr, &frameBuffer[i]) != VK_SUCCESS) {
      return false;
    }
  }
  return true;
}

bool VulkanRenderer::CreateSynch()
{
  VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  VkFenceCreateInfo     fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };

  return vkCreateFence(device->device, &fenceInfo, nullptr, &inFlightFence) == VK_SUCCESS
         && vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &imageMutex) == VK_SUCCESS
         && vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &renderMutex) == VK_SUCCESS;
}

bool VulkanRenderer::CreateCommandPool()
{
  VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = device->graphicsQueue->index;

  return vkCreateCommandPool(device->device, &poolInfo, nullptr, &commandPool) == VK_SUCCESS;
}

bool VulkanRenderer::CreateCommandBuffers()
{
  VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  return vkAllocateCommandBuffers(device->device, &allocInfo, &command) == VK_SUCCESS;
}

bool VulkanRenderer::RecreateSwapChain()
{
  return vkDeviceWaitIdle(device->device) && swapchain->Rebuild(physicalDevice, device)
         && swapchain->LoadImageViews(device) && CreateFrameBuffers();
}

bool VulkanRenderer::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
  VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device->device, &allocInfo, &commandBuffer);

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

  if (vkQueueSubmit(device->graphicsQueue->queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    return false;
  }

  if (vkQueueWaitIdle(device->graphicsQueue->queue) != VK_SUCCESS) {
    return false;
  }

  vkFreeCommandBuffers(device->device, commandPool, 1, &commandBuffer);

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

  if (vkCreateBuffer(device->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    return nullptr;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device->device, buffer, &memRequirements);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice->device, &memProperties);

  Uint32 index = 0;
  for (Uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
    const bool isMemoryBits = memRequirements.memoryTypeBits & (1 << i);
    const bool hasRequiredProperties = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;
    if (isMemoryBits && hasRequiredProperties) {
      index = i;
      break;
    }
  }
  if (!index) {
    return nullptr;
  }

  VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = index;

  if (vkAllocateMemory(device->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
    return nullptr;
  }

  if (vkBindBufferMemory(device->device, buffer, bufferMemory, 0) != VK_SUCCESS) {
    return nullptr;
  }

  return buffer;
}

bool VulkanRenderer::CreateVertexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data)
{
  VkDeviceMemory bufferMemory;
  VkBuffer       stageBuffer = CreateBuffer(size,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      bufferMemory);
  if (!stageBuffer) {
    return false;
  }
  void* dst;
  if (vkMapMemory(device->device, bufferMemory, 0, size, 0, &dst) != VK_SUCCESS) {
    return false;
  }
  memcpy(dst, data, size);
  vkUnmapMemory(device->device, bufferMemory);
  buffer = CreateBuffer(size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        memory);
  if (!buffer) {
    return false;
  }
  if (!CopyBuffer(stageBuffer, buffer, size)) {
    return false;
  }
  vkDestroyBuffer(device->device, stageBuffer, nullptr);
  vkFreeMemory(device->device, bufferMemory, nullptr);
  return true;
}

bool VulkanRenderer::CreateIndexBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize size, void* data)
{
  VkDeviceMemory bufferMemory;
  VkBuffer       stageBuffer = CreateBuffer(size,
                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      bufferMemory);
  if (!stageBuffer) {
    return false;
  }
  void* dst;
  if (vkMapMemory(device->device, bufferMemory, 0, size, 0, &dst) != VK_SUCCESS) {
    return false;
  }
  memcpy(dst, data, size);
  vkUnmapMemory(device->device, bufferMemory);
  buffer = CreateBuffer(size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        memory);
  if (!buffer) {
    return false;
  }
  if (!CopyBuffer(stageBuffer, buffer, size)) {
    return false;
  }
  vkDestroyBuffer(device->device, stageBuffer, nullptr);
  vkFreeMemory(device->device, bufferMemory, nullptr);
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
  renderPassInfo.renderArea.extent = swapchain->extent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkViewport viewport{ 0.0f, 0.0f, (float)swapchain->extent.width, (float)swapchain->extent.height, 0.f, 1.f };
  vkCmdSetViewport(command, 0, 1, &viewport);

  VkRect2D scissor{ { 0, 0 }, swapchain->extent };
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
  colorAttachment.format = swapchain->format.format;
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

  return vkCreateRenderPass(device->device, &renderPassInfo, nullptr, &renderPass) == VK_SUCCESS;
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

bool VulkanRenderer::PrepareRendering()
{
  return CreateRenderPipeline() && CreateFrameBuffers() && CreateCommandBuffers() && CreateSynch();
}

}  // namespace NycaTech::Renderer