//
// Created by rplaz on 2023-12-03.
//

#define _USE_MATH_DEFINES
#include "obj_model.h"

#include <vulkan/vulkan.h>

#include <cmath>
#include <iostream>

namespace NycaTech {

ObjModel::ObjModel()
{
}

bool ObjModel::Rotate(Float32 yaw, Float32 pitch, Float32 roll)
{
  const Float32 cosYaw = cos(yaw), sinYaw = sin(yaw);
  const Float32 cosPitch = cos(pitch), sinPitch = sin(pitch);
  const Float32 cosRoll = cos(roll), sinRoll = sin(roll);

  for (Uint32 i = 0; i < vertices.Count(); i += 3) {
    Float32 x = vertices[i + 0], y = vertices[i + 1], z = vertices[i + 2];

    vertices[i + 0] = ((x * cosYaw) - (y * sinYaw)) + ((x * cosPitch) + (z * sinPitch));
    vertices[i + 1] = ((x * sinYaw) + (y * cosYaw)) + ((y * cosRoll) - (z * sinRoll));
    vertices[i + 2] = ((x * sinPitch) - (z * cosPitch)) + ((y * sinRoll) + (z * cosRoll));
  }

  return true;
}

bool ObjModel::Move(Float32 x, Float32 y, Float32 z)
{
  for (Uint32 i = 0; i < vertices.Count(); i += 3) {
    vertices[i + 0] += x;
    vertices[i + 1] += y;
    vertices[i + 2] += z;
  }
  return true;
}

bool ObjModel::Scale(Float32 x, Float32 y, Float32 z)
{
  for (Uint32 i = 0; i < vertices.Count(); i += 3) {
    vertices[i + 0] *= x;
    vertices[i + 1] *= y;
    vertices[i + 2] *= z;
  }
  return true;
}

ObjModel* ObjModel::FromFile(const char* file_path)
{
  tinyobj::ObjReader reader;
  ObjModel*          model = new ObjModel();
  if (!reader.ParseFromFile(file_path)) {
    return nullptr;
  }

  for (const auto& shape : reader.GetShapes()) {
    for (const auto& index : shape.mesh.indices) {
      model->indices.Insert(index.vertex_index);
    }
  }

  for (const auto& vertex : reader.GetAttrib().vertices) {
    model->vertices.Insert(vertex);
  }
  return model;
}

VkVertexInputBindingDescription ObjModel::GetVkVertexInputBindingDescription()
{
  return { .binding = 0, .stride = sizeof(Float32) * 3, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };
}

VkVertexInputAttributeDescription ObjModel::GetVkVertexInputAttributeDescription()
{
  return { .location = 0, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = 0 };
}

}  // namespace NycaTech
