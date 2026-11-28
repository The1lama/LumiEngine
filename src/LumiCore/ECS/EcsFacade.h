#pragma once

#include "../Utility/Singleton.h"
#include "Components.h"
#include "ECSTypes.h"
#include "Entity.h"
#include "System.h"

#include <memory>

namespace LumiCore::ECS
{
  class EntityManager;
  // class ComponentManager;
  class SystemsManager;

  class ECSFacade : public Utility::Function::Singleton<ECSFacade>
  {
  private:
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<SystemsManager> m_systemsManager;

  public:
    ECSFacade();
    ~ECSFacade();

    // Entity
    void Initialize();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    // Component
    template <typename T>
    void RegisterComponent();

    template <typename T>
    void AddComponent(Entity entity, T component);

    template <typename T>
    inline ComponentType GetComponentType();

    template <typename T>
    void RemoveComponent(Entity entity);

    template <typename T>
    T& GetComponent(Entity entity);

    template <typename T>
    bool EntityHasComponent(Entity entity);

    // Systems
    template <typename T>
    std::shared_ptr<T> RegisterSystem();

    template <typename T>
    void SetSystemSignature(Signature signature);
  };
  template <typename T>
  inline bool ECSFacade::EntityHasComponent(Entity entity)
  {
    return m_entityManager->GetSignature(entity).test(GetComponentType<T>());
  }
#pragma region Component

  template <typename T>
  inline void LumiCore::ECS::ECSFacade::RegisterComponent()
  {
    m_componentManager->RegisterComponent<T>();
  }

  template <typename T>
  inline void ECSFacade::AddComponent(Entity entity, T component)
  {
    m_componentManager->AddComponent(entity, component);

    auto signature = m_entityManager->GetSignature(entity);
    signature.set(m_componentManager->GetComponentType<T>(), true);

    m_entityManager->SetSignature(entity, signature);
    m_systemsManager->EntitySignatureChanged(entity, signature);
  }

  template <typename T>
  inline T& ECSFacade::GetComponent(Entity entity)
  {
    return m_componentManager->GetComponent<T>(entity);
  }

  template <typename T>
  inline ComponentType ECSFacade::GetComponentType()
  {
    return m_componentManager->GetComponentType<T>();
  }


  template <typename T>
  inline void ECSFacade::RemoveComponent(Entity entity)
  {
    m_componentManager->RemoveComponent<T>(entity);
    auto signature = m_entityManager->GetSignature(entity);
    signature.set(m_componentManager->GetComponentType<T>(), false);

    m_entityManager->SetSignature(entity, signature);
    m_systemsManager->EntitySignatureChanged(entity, signature);
  }


#pragma endregion // Component

#pragma region System

  template <typename T>
  inline std::shared_ptr<T> ECSFacade::RegisterSystem()
  {
    return m_systemsManager->RegisterSystem<T>();
  }

  template <typename T>
  inline void ECSFacade::SetSystemSignature(Signature signature)
  {
    m_systemsManager->SetSignature<T>(signature);

    for (auto entity : m_entityManager->GetAliveEntities())
    {
      m_systemsManager->EntitySignatureChanged(entity, m_entityManager->GetSignature(entity));
    }
  }

#pragma endregion // System

} // namespace LumiCore::ECS


