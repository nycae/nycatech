//
// Created by rplaz on 2023-12-16.
//

#include "shader.h"

#include <lib/assert.h>

namespace NycaTech::Renderer {

Shader::Shader(Type type, const char* filePath)
    : type(type)
{
  FILE* fd = fopen(filePath, "rb");
  Assert(fd, "unable to open file");
  fseek(fd, 0, SEEK_END);
  length = ftell(fd);
  rewind(fd);
  content = (char*)malloc(length);
  fread(content, 1, length, fd);
  fclose(fd);
}

Shader::~Shader()
{
  free(content);
}

}  // namespace NycaTech::Renderer