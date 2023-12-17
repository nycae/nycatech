//
// Created by rplaz on 2023-12-04.
//

#include "entity.h"

namespace NycaTech {


Vector<Component*>& Entity::ComponentsOfType(Component::Type type)
{
  return components[type];
}

Entity::~Entity()
{
  for (const auto [key, component_vector] : components) {
    for (const auto& component : component_vector) {
      delete component;
    }
  }
}

Entity::Builder::Builder()
    : toBuild(new Entity)
{
}

Entity::Builder& Entity::Builder::AddComponent(Component* component)
{
  toBuild->components[component->get_type()].Insert(component);
  return Self;
}

Entity* Entity::Builder::Build()
{
  return toBuild;
}

}  // namespace NycaTech