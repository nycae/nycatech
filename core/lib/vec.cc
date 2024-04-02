//
// Created by rplaz on 2024-02-21.
//

#include "vec.h"

namespace NycaTech {

Vec2::Vec2(float x, float y)
    : x(x), y(y)
{
}

Vec2 Vec2::operator=(const Vec2& other)
{
  Self.x = other.x;
  Self.y = other.y;

  return Self;
}

Vec2 Vec2::operator+(const Vec2& other) const
{
  return Vec2(Self.x + other.x, Self.y + other.y);
}

Vec2 Vec2::operator+=(const Vec2& other)
{
  return Self = Self + other;
}

Vec2 Vec2::operator-(const Vec2& other) const
{
  return Vec2(Self.x - other.x, Self.y - other.y);
}

Vec2 Vec2::operator-=(const Vec2& other)
{
  return Self = Self - other;
}

Vec2 Vec2::operator*(const float& other) const
{
  return Vec2(Self.x * other, Self.y * other);
}

Vec2 Vec2::operator*=(const float& other)
{
  return Self = Self * other;
}

Vec2 Vec2::operator/(const float& other) const
{
  return Vec2(Self.x / other, Self.y / other);
}

Vec2 Vec2::operator/=(const float& other)
{
  return Self = Self / other;
}

Vec3::Vec3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

Vec4::Vec4(float x, float y, float z, float w)
    : x(x), y(y), z(z), w(w)
{
}

}  // namespace NycaTech