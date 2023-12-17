//
// Created by rplaz on 2023-12-04.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "component.h"
#include "lib/vector.h"

namespace NycaTech {

class Entity final {
public:
  class Builder final {
  public:
     Builder();
    ~Builder() = default;

  public:
    Builder& AddComponent(Component* component);
    Entity*  Build();

  private:
    Entity* toBuild;
  };

  ~Entity();

public:
  Vector<Component*>& ComponentsOfType(Component::Type);

private:
  explicit Entity() = default;

private:
  HashMap<Component::Type, Vector<Component*>> components;
};

}  // namespace NycaTech

#endif  // ENTITY_H
