#pragma once

#include "ECSTypes.h"

#include <memory>
#include <set>
#include <unordered_map>

namespace LumiCore::ECS
{
  class BaseSystem
  {
  public:
    std::set<Entity> Entities;
  };

  class SystemsManager
  {
  private:
    std::unordered_map<const char*, Signature> m_signatures;
    std::unordered_map<const char*, std::shared_ptr<BaseSystem>> m_systems;

  public:
    template <typename T>
    std::shared_ptr<T> RegisterSystem();

    template <typename T>
    void SetSignature(Signature signature);

    void EntityDestoryed(Entity entity);

    void EntitySignatureChanged(Entity entity, Signature entitySignature);
  };

  template <typename T>
  inline std::shared_ptr<T> SystemsManager::RegisterSystem()
  {
    const char* typeName = typeid(T).name();

    auto system = std::make_shared<T>();
    m_systems.emplace(typeName, system);
    return system;
  }
  template <typename T>
  inline void SystemsManager::SetSignature(Signature signature)
  {
    const char* typeName = typeid(T).name();
    m_signatures[typeName] = signature;
  }


} // namespace LumiCore::ECS
