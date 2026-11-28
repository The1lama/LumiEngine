#include "EcsFacade.h"

#include <memory>


namespace LumiCore::ECS
{
  ECSFacade::ECSFacade() = default;
  ECSFacade::~ECSFacade() = default;

#pragma region Entity

  void ECSFacade::Initialize()
  {
    m_entityManager = std::make_unique<EntityManager>();
    m_componentManager = std::make_unique<ComponentManager>();
    m_systemsManager = std::make_unique<SystemsManager>();
  }

  Entity ECSFacade::CreateEntity()
  {
    return m_entityManager->CreateEntity();
  }

  void ECSFacade::DestroyEntity(Entity entity)
  {
    m_entityManager->DestroyEntity(entity);
    m_componentManager->EntityDestoryed(entity);
    m_systemsManager->EntityDestoryed(entity);
  }


#pragma endregion // entity


} // namespace LumiCore::ECS
