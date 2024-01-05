// #define NO_SDL_GLEXT
// #include <GL/glew.h>
// #include <SDL.h>
// #undef main

#include <iostream>

#include "lib/assert.h"
#include "renderer/obj_model.h"
#include "renderer/vulkan_renderer.h"

using namespace NycaTech;
using namespace NycaTech::Renderer;

int main(int argc, char* argv[])
{
  VulkanRenderer renderer;

  ObjModel* teapot = ObjModel::FromFile("../assets/teapot.obj");
  Shader    vertexShader(Shader::Type::VERTEX, "../assets/vert.spv");
  Shader    fragmentShader(Shader::Type::VERTEX, "../assets/frag.spv");

  Assert(teapot, "unable to load assets");
  Assert(renderer.AttachShader(vertexShader) && renderer.AttachShader(fragmentShader), "unable to attach assets!");

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
    // Assert(renderer.DrawFrame(), "Error drawing frames");
    sleep_until(last_frame + milliseconds(12));
  }
  return EXIT_SUCCESS;
}
