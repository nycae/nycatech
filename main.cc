// #define NO_SDL_GLEXT
// #include <GL/glew.h>
// #include <SDL.h>
// #undef main

#include <iostream>

#include "core\renderer\swapchain.h"
#include "lib/assert.h"
#include "renderer/device.h"
#include "renderer/obj_model.h"
#include "renderer/vulkan_renderer.h"

using namespace NycaTech;
using namespace NycaTech::Renderer;

int main(int argc, char* argv[])
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "unable to init sdl: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  auto window = SDL_CreateWindow("NycaTech", 200, 200, 1600, 900, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "unable to create window: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  VulkanInstance* instance = VulkanInstance::Create(window);
  PhysicalDevice* physicalDevice = PhysicalDevice::SuitableDevices(instance)[0];
  Surface*        surface = Surface::Create(instance, window);
  Device*         device = Device::Create(physicalDevice, surface);
  SwapChain*      swapchain = SwapChain::Create(physicalDevice, surface, device);

  Assert(instance, "invalid instance");
  Assert(physicalDevice, "invalid physical device");
  Assert(surface, "invalid surface");
  Assert(device, "invalid device");
  Assert(swapchain, "invalid swapchain")

  VulkanRenderer* renderer = VulkanRenderer::Create(physicalDevice, swapchain, device);

  ObjModel* teapot = ObjModel::FromFile("../assets/teapot.obj");
  Shader*   vertexShader = Shader::VertexFromFile("../assets/vert.spv", device->device);
  Shader*   fragmentShader = Shader::FragmentFromFile("../assets/frag.spv", device->device);

  teapot->Scale(0.2, 0.3, 0.3);
  // teapot->Rotate(2.8, 0.0, 0.0);
  teapot->Move(0.0, -0.5, 0.2);

  if (!teapot || !vertexShader || !fragmentShader) {
    throw RuntimeError("unable to load assets!");
  }

  if (!renderer->AttachShader(vertexShader) || !renderer->AttachShader(fragmentShader)
      || !renderer->LoadModel(teapot)) {
    throw RuntimeError("unable to attach assets!");
  }

  if (!renderer->PrepareRendering()) {
    throw RuntimeError("unable to setup renderer!");
  }

  auto running = true;
  auto last_frame = Time::now();
  while (running) {
    auto const elapsed_ms = duration_cast<milliseconds>(Time::now() - last_frame).count();
    auto const delta = static_cast<float>(elapsed_ms) / 1000.0f;
    SDL_Event  event;
    last_frame = Time::now();
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        running = !running;
      }
    }
    if (!renderer->DrawFrame()) {
      throw RuntimeError("Error drawing frames");
    }
    sleep_until(last_frame + milliseconds(12));
  }
  return EXIT_SUCCESS;
}
