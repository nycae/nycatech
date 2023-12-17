//
// Created by rplaz on 2023-12-04.
//

#include "component.h"

namespace NycaTech {

Component::Component(const Type type)
  : type(type)
{
}

bool Component::is_a(Type other) const
{
  const auto this_type = static_cast<Uint32>(type);
  const auto other_type = static_cast<Uint32>(other);
  return this_type & other_type;
}

Component::Type Component::get_type() const
{
  return type;
}

}  // namespace NycaTech