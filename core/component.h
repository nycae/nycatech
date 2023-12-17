//
// Created by rplaz on 2023-12-04.
//

#ifndef COMPONENT_H
#define COMPONENT_H

#include "lib/types.h"

namespace NycaTech {

class Component {
public:
  enum class Type : Uint32 {
    None = 1 << 0,
    Render = 1 << 1,
    AI = 1 << 2,
    Network = 1 << 3,
    Physics = 1 << 4,
    MAX = 1 << 31 - 1,
  };

  explicit Component(Type type);

  bool is_a(Type other) const;
  Type get_type() const;

private:
  Type type;
};

}  // namespace NycaTech

#endif  // COMPONENT_H
