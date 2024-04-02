//
// Created by rplaz on 2024-04-02.
//

#ifndef NYCATECH_MATRIX_H
#define NYCATECH_MATRIX_H

#include <lib/pair.h>
#include <lib/types.h>

namespace NycaTech::Math {

template <Uint64 Rows, Uint64 Cols>
class Matrix final {
  using Position = NycaTech::Pair<Uint64, Uint64>;

public:
  Matrix();
  ~Matrix();

  Float64&       operator[](const Position& position);
  const Float64& operator[](const Position& position) const;

  Matrix& operator+=(Float64 n);
  Matrix& operator+=(const Matrix<Rows, Cols>& other);
  Matrix& operator-=(Float64 n);
  Matrix& operator-=(const Matrix<Rows, Cols>& other);
  Matrix& operator*=(Float64 n);
  Matrix& operator*=(const Matrix<Rows, Cols>& other);
  Matrix& operator/=(Float64 n);
  Matrix& operator/=(const Matrix<Rows, Cols>& other);

  Float64&       at(const Position& position);
  const Float64& at(const Position& position) const;
  Float64&       at(Uint64 x, Uint64 y);
  const Float64& at(Uint64 x, Uint64 y) const;

  Float64*       Data();
  const Float64* Data() const;

private:
  Float64* data;
};

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator/=(const Matrix<Rows, Cols>& other)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] /= other.data;
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator/=(Float64 n)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] /= n;
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator*=(const Matrix<Rows, Cols>& other)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] *= other.data[i];
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator*=(Float64 n)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] *= n;
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator-=(const Matrix<Rows, Cols>& other)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] -= other.data[i];
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator-=(Float64 n)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] -= n;
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator+=(Float64 n)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] += n;
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>& Matrix<Rows, Cols>::operator+=(const Matrix<Rows, Cols>& other)
{
  for (Uint64 i = 0; i < Rows * Cols; i++) {
    Self.data[i] += other.data[i];
  }
  return Self;
}

template <Uint64 Rows, Uint64 Cols>
Float64& Matrix<Rows, Cols>::at(const Matrix::Position& position)
{
  return Self[position];
}

template <Uint64 Rows, Uint64 Cols>
Float64& Matrix<Rows, Cols>::at(Uint64 x, Uint64 y)
{
  return Self[{ x, y }];
}

template <Uint64 Rows, Uint64 Cols>
const Float64& Matrix<Rows, Cols>::at(Uint64 x, Uint64 y) const
{
  return Self[{ x, y }];
}

template <Uint64 Rows, Uint64 Cols>
const Float64& Matrix<Rows, Cols>::at(const Matrix::Position& position) const
{
  return Self[position];
}

template <Uint64 Rows, Uint64 Cols>
const Float64& Matrix<Rows, Cols>::operator[](const Matrix::Position& position) const
{
  return data[position.first * Rows + position.second];
}

template <Uint64 Rows, Uint64 Cols>
Float64& Matrix<Rows, Cols>::operator[](const Matrix::Position& position)
{
  return data[position.first * Rows + position.second];
}

template <Uint64 Rows, Uint64 Cols>
const Float64* Matrix<Rows, Cols>::Data() const
{
  return data;
}

template <Uint64 Rows, Uint64 Cols>
Float64* Matrix<Rows, Cols>::Data()
{
  return data;
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>::~Matrix()
{
  data = (Float64*)malloc(Rows * Cols * sizeof(Float64));
}

template <Uint64 Rows, Uint64 Cols>
Matrix<Rows, Cols>::Matrix()
{
  free(data);
}

}  // namespace NycaTech::Math

#endif  // NYCATECH_MATRIX_H
