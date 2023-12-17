//
// Created by rplaz on 2023-12-03.
//

#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H

#include <vulkan/vulkan.h>

#include "lib/types.h"
#include "lib/vector.h"
#include "tiny_obj_loader.h"

namespace NycaTech {

class ObjModel final {
private:
  explicit ObjModel();

public:
  ObjModel(ObjModel&&) = delete;
  ObjModel(const ObjModel&) = delete;
  ObjModel(const char* file_path) = delete;

public:
  bool Rotate(Float32, Float32, Float32);
  bool Move(Float32, Float32, Float32);
  bool Scale(Float32, Float32, Float32);

public:
  static ObjModel*                         FromFile(const char* file_path);
  static VkVertexInputBindingDescription   GetVkVertexInputBindingDescription();
  static VkVertexInputAttributeDescription GetVkVertexInputAttributeDescription();

public:
  VkBuffer        vertexBuffer;
  VkDeviceMemory  vertexMemory;
  VkBuffer        indexBuffer;
  VkDeviceMemory  indexMemory;
  Vector<Uint32>  indices;
  Vector<Float32> vertices;
};

};  // namespace NycaTech

#endif  // OBJ_MODEL_H
