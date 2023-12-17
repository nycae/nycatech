//
// Created by rplaz on 2023-12-13.
//

#ifndef VECTOR_H
#define VECTOR_H

#include "types.h"
#include <cstdlib>
#include <functional>

namespace NycaTech {

#ifndef INLINE_LIB
#define INLINE_LIB inline
#endif

template <typename T>
class Vector {
public:
  INLINE_LIB explicit Vector(Uint32 count = 0);
  INLINE_LIB explicit Vector(std::initializer_list<T> init);

  INLINE_LIB Vector(Vector&& other);
  INLINE_LIB Vector(const Vector& other);

  INLINE_LIB Vector& operator=(Vector&& other);
  INLINE_LIB Vector& operator=(const Vector& other);
  INLINE_LIB Vector& operator=(std::vector<T>);

public:
  INLINE_LIB bool     Resize(Uint32 newSize);
  INLINE_LIB bool     Insert(const T& element);
  INLINE_LIB bool     At(Uint32 index, T* elem) const;
  INLINE_LIB bool     At(Uint32 index, T** elem) const;
  INLINE_LIB bool     Emplace(Uint32 index, const T& elem);
  INLINE_LIB bool     AdjustSize();
  INLINE_LIB T&       operator[](Uint32 index);
  INLINE_LIB const T& operator[](Uint32 index) const;
  INLINE_LIB Uint32   Capacity() const;
  INLINE_LIB Uint32&  CountMut();
  INLINE_LIB Uint32   Count() const;
  INLINE_LIB void     OverrideCount(Uint32 newSize);
  INLINE_LIB T*       Data();
  INLINE_LIB const T* Data() const;
  INLINE_LIB bool     Contains(const T& other) const;
  INLINE_LIB bool     Contains(std::function<bool(const T&)>) const;
  INLINE_LIB bool     IsEmpty() const;
  INLINE_LIB Uint32   ElemSize() const;

public:
  INLINE_LIB T*       begin();
  INLINE_LIB const T* begin() const;
  INLINE_LIB T*       end();
  INLINE_LIB const T* end() const;

private:
  T*     data;
  Uint32 count;
  Uint32 size;
};

template <typename T>
INLINE_LIB Vector<T>::Vector(Uint32 initCount)
    : size(initCount), count(initCount), data((T*)malloc(sizeof(T) * initCount))
{
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> init)
{
  for (const auto& elem : init) {
    this->Insert(elem);
  }
}
template <typename T>
Vector<T>::Vector(Vector&& other)
{
  *this = (Vector&&) other;
}

template <typename T>
Vector<T>::Vector(const Vector& other)
{
  *this = other;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other)
{
  *this = other;
  delete &other;
  return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other)
{
  const auto byteSize = other.Count() * sizeof(T);
  size = other.size;
  count = other.count;
  data = (T*)malloc(byteSize);
  memcpy(data, other.data, byteSize);
  return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(std::vector<T> other)
{
  data = (T*)malloc(sizeof(T) * other.size());
  count = other.size();
  size = other.capacity();
  memcpy(data, other.data(), sizeof(T) * other.size());
  return *this;
}

template <typename T>
INLINE_LIB bool Vector<T>::Insert(const T& element)
{
  if (count >= size) {
    const Uint32 newCapacity = size * 1.62;
    Resize(newCapacity + (16 - (newCapacity % 16)));
  }
  data[count] = element;
  count++;
  return true;
}

template <typename T>
INLINE_LIB bool Vector<T>::At(Uint32 index, T* elem) const
{
  if (index >= count) {
    return false;
  }

  *elem = data[index];
  return true;
}

template <typename T>
bool Vector<T>::At(Uint32 index, T** elem) const
{
  if (index >= count) {
    return false;
  }

  *elem = &data[index];
  return true;
}

template <typename T>
bool Vector<T>::Emplace(Uint32 index, const T& elem)
{
  if (index >= count) {
    return false;
  }
  data[index] = elem;
  return true;
}

template <typename T>
bool Vector<T>::AdjustSize()
{
  return Resize(count);
}

template <typename T>
T& Vector<T>::operator[](Uint32 index)
{
  return data[index];
}

template <typename T>
const T& Vector<T>::operator[](Uint32 index) const
{
  return data[index];
}

template <typename T>
Uint32 Vector<T>::Capacity() const
{
  return size;
}

template <typename T>
Uint32 Vector<T>::Count() const
{
  return count;
}

template <typename T>
void Vector<T>::OverrideCount(Uint32 newSize)
{
  count = newSize;
}

template <typename T>
Uint32& Vector<T>::CountMut()
{
  return count;
}

template <typename T>
T* Vector<T>::Data()
{
  return data;
}

template <typename T>
const T* Vector<T>::Data() const
{
  return data;
}

template <typename T>
bool Vector<T>::Contains(const T& other) const
{
  for (Uint32 i = 0; i < count; i++) {
    if (data[i] == other) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool Vector<T>::Contains(std::function<bool(const T&)> found) const
{
  for (Uint32 i = 0; i < count; i++) {
    if (found(data[i])) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool Vector<T>::IsEmpty() const
{
  return count <= 0;
}

template <typename T>
Uint32 Vector<T>::ElemSize() const
{
  return sizeof(T);
}

template <typename T>
T* Vector<T>::begin()
{
  return data;
}

template <typename T>
const T* Vector<T>::begin() const
{
  return data;
}

template <typename T>
T* Vector<T>::end()
{
  return data + count;
}

template <typename T>
const T* Vector<T>::end() const
{
  return data + count;
}

template <typename T>
INLINE_LIB bool Vector<T>::Resize(const Uint32 newSize)
{
  if (newSize < count) {
    return false;
  }
  if (newSize == size) {
    return true;
  }

  T* oldData = data;
  T* newData = (T*)malloc(sizeof(T) * newSize);
  memcpy(newData, oldData, sizeof(T) * count);
  free(oldData);

  size = newSize;
  data = newData;
  return true;
}

}  // namespace NycaTech

#endif  // VECTOR_H
