//
// Created by rplaz on 2023-12-04.
//

#ifndef WORLD_H
#define WORLD_H

#include "entity.h"
#include "system.h"
#include "types.h"

namespace NycaTech {

class World final {
public:
   World() = default;
   World(World&&) = delete;
   World(const World&) = delete;
  ~World();

public:
  void Tick(float delta);
  void Run();
  void Restart();
  void Stop();

public:
  void AddSystem(System* system);
  void AddEntity(Entity* entity);

private:
  Vector<Entity*> entities;
  Vector<System*> systems;
  bool            should_tick = true;
};

}  // namespace NycaTech
#endif  // WORLD_H
