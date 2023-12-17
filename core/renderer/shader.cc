//
// Created by rplaz on 2023-12-16.
//

#include "shader.h"

namespace NycaTech::Renderer {

Shader::Shader(Type type, VkDevice device)
    : type(type), uploadedTo(device)
{
}

Shader::~Shader()
{
  vkDestroyShaderModule(uploadedTo, module, nullptr);
}

Shader* Shader::VertexFromFile(const char* filepath, VkDevice device)
{
  Shader* shader = new Shader(VERTEX, device);
  if (!FromFile(filepath, shader)) {
    delete shader;
    return nullptr;
  }
  return shader;
}

Shader* Shader::FragmentFromFile(const char* filepath, VkDevice device)
{
  Shader* shader = new Shader(FRAGMENT, device);
  if (!FromFile(filepath, shader)) {
    delete shader;
    return nullptr;
  }
  return shader;
}

bool Shader::FromFile(const char* filepath, Shader* shader)
{
  FILE* fd = fopen(filepath, "rb");
  if (!fd) {
    return false;
  }

  fseek(fd, 0, SEEK_END);
  Uint64 length = ftell(fd);
  rewind(fd);
  char* content = (char*)malloc(length);
  fread(content, 1, length, fd);
  fclose(fd);

  VkShaderModuleCreateInfo info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
  info.codeSize = length;
  info.pCode = (Uint32*)content;

  if (vkCreateShaderModule(shader->uploadedTo, &info, nullptr, &shader->module) != VK_SUCCESS) {
    return false;
  };

  free(content);
  return true;
}

}  // namespace NycaTech::Renderer