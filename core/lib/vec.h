#pragma once

#include "types.h"

namespace NycaTech {

struct Vec2 {
  explicit Vec2(float x = 0, float y = 0);

  Vec2 operator=(const Vec2& other);

  Vec2 operator+(const Vec2& other) const;
  Vec2 operator+=(const Vec2& other);
  Vec2 operator-(const Vec2& other) const;
  Vec2 operator-=(const Vec2& other);

  Vec2 operator*(const float& other) const;
  Vec2 operator*=(const float& other);
  Vec2 operator/(const float& other) const;
  Vec2 operator/=(const float& other);

  Vec2 operator*(const Vec2& other) = delete;
  Vec2 operator*=(const Vec2& other) = delete;
  Vec2 operator/(const Vec2& other) = delete;
  Vec2 operator/=(const Vec2& other) = delete;

  float x, y;
};

struct Vec3 {
  explicit Vec3(float x = 0, float y = 0, float z = 0);
  float x, y, z;
};

struct Vec4 {
  explicit Vec4(float x = 0, float y = 0, float z = 0, float w = 0);
  float x, y, z, w;
};

}  // namespace NycaTech