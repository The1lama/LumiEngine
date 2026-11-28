#include "Components.h"

#include <cassert>

namespace LumiCore::ECS
{

} // namespace LumiCore::ECS


namespace LumiCore::ECS
{

  void ComponentManager::EntityDestoryed(Entity entity)
  {
    for (auto const& pair : m_componentArrays)
    {
      auto const& component = pair.second;
      component->EntityDestroyed(entity);
    }
  }

} // namespace LumiCore::ECS
