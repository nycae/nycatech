//
// Created by rplaz on 2023-12-16.
//

#ifndef SHADER_H
#define SHADER_H

#include "lib/types.h"

namespace NycaTech::Renderer {

struct Shader {
  enum Type : Uint32 {
    VERTEX = 0x00000001,
    FRAGMENT = 0x00000002,
  };

  explicit Shader(Type type, const char* filePath);
  ~        Shader();

  Type   type;
  char*  content;
  Uint64 length;
};

}  // namespace NycaTech::Renderer

#endif  // SHADER_H
