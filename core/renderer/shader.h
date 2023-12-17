//
// Created by rplaz on 2023-12-16.
//

#ifndef SHADER_H
#define SHADER_H
#include <vulkan/vulkan_core.h>

#include "types.h"

namespace NycaTech::Renderer {

class Shader {
public:
  enum Type : Uint32 {
    VERTEX = 0x00000001,
    FRAGMENT = 0x00000002,
  };

private:
  explicit Shader(Type, VkDevice);

public:
  VkShaderModule module;
  VkDevice       uploadedTo;
  Type           type;

public:
  ~              Shader();
  static Shader* VertexFromFile(const char* filepath, VkDevice device);
  static Shader* FragmentFromFile(const char* filepath, VkDevice device);

private:
  static bool FromFile(const char* filepath, Shader* shader);
};

}  // namespace NycaTech::Renderer

#endif  // SHADER_H
