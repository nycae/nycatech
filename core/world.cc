//
// Created by rplaz on 2023-12-04.
//

#include "world.h"

namespace NycaTech {

World::~World()
{
  for (Entity* entity: entities) {
    delete entity;
  }
}

void World::Tick(const float delta)
{
  for (System* system: systems) {
    system->Run(entities, delta);
  }
}

void World::AddSystem(System* system)
{
  systems.Insert(system);
}

void World::AddEntity(Entity* entity)
{
  entities.Insert(entity);
}

}  // namespace NycaTech