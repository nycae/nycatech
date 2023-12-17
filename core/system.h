//
// Created by rplaz on 2023-12-04.
//

#ifndef SYSTEM_H
#define SYSTEM_H

#include "entity.h"
#include "types.h"

namespace NycaTech {

class System {
public:
  virtual ~    System() = default;
  virtual void Run(Vector<Entity*>& entities, float delta) = 0;
};

}  // namespace NycaTech

#endif  // SYSTEM_H
