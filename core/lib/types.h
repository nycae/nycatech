#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <chrono>
#include <fstream>
#include <map>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace NycaTech {

#define Self (*this)
#define Inline inline

using namespace std::chrono;
using namespace std::this_thread;

typedef high_resolution_clock Time;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename K, typename V>
using RedBlackMap = std::map<K, V>;

template <typename T>
using StreamIterator = std::istreambuf_iterator<T>;

using Thread = std::thread;
using StreamReader = std::ifstream;
using StringStream = std::stringstream;

using Uint8 = uint8_t;
using Uint16 = uint16_t;
using Uint32 = uint32_t;
using Uint64 = uint64_t;
using Int32 = int32_t;
using Int64 = int64_t;
using Float32 = float;
using Float64 = double;
using String = std::string;
using Exception = std::exception;
using RuntimeError = std::runtime_error;

using Vect3 = std::array<float, 3>;
using Quad = std::array<float, 4>;

struct Transform {
  Transform(Transform&&) = default;
  Transform(const Transform&) = default;

  Transform()
      : position(), rotation(), scale(){};
  Transform(const Vect3& position, const Quad& rotation, const Vect3& scale)
      : position(position), rotation(rotation), scale(scale){};

  Vect3 scale;
  Quad  rotation;
  Vect3 position;
};

}  // namespace NycaTech

#endif  // TYPES_H
