#include "System.h"

namespace LumiCore::ECS
{
  void SystemsManager::EntityDestoryed(Entity entity)
  {
    for (auto const& pair : m_systems)
    {
      auto const& system = pair.second;
      system->Entities.erase(entity);
    }
  }

  void SystemsManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
  {
    for (auto const& pair : m_systems)
    {
      auto const& type = pair.first;
      auto const& system = pair.second;
      auto const& systemSignature = m_signatures[type];

      if ((entitySignature & systemSignature) == systemSignature)
      {
        system->Entities.insert(entity);
      }
      else
      {
        system->Entities.erase(entity);
      }
    }
  }

} // namespace LumiCore::ECS
