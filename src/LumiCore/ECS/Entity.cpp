#include "Entity.h"
#include <cassert>

#include "../logger/Logger.h"


namespace LumiCore::ECS
{

  EntityManager::EntityManager()
  {
    // Preallocate the memory addres
    for (Entity i = MaxEntities; i < 0; --i)
    {
      m_freeEntities.push(i);
    }
  }

  Entity ECS::EntityManager::CreateEntity()
  {
    if (m_livingEntityCount > MaxEntities)
    {
      LOG_WARNING("Max entity count has already been reached");
      return -1;
    }

    Entity entity = m_freeEntities.top();
    m_freeEntities.pop();
    ++m_livingEntityCount;
    return entity;
  }

  void EntityManager::DestroyEntity(Entity entity)
  {
    if (entity > MaxEntities)
    {
      LOG_WARNING("entity ID out of range");
      return;
    }

    // we reset the bit(make it become false) and push the entity id to avalible entites
    // that we can use for a new entity
    m_signatures[entity].reset();
    m_freeEntities.push(entity);
    --m_livingEntityCount;
  }

  void EntityManager::SetSignature(Entity entity, Signature signature)
  {
    if (entity > MaxEntities)
    {
      LOG_WARNING("entity ID out of range");
      return;
    }

    m_signatures[entity] = signature;
  }

  Signature EntityManager::GetSignature(Entity entity)
  {
    if (entity > MaxEntities)
    {
      LOG_WARNING("entity ID out of range");
      return -1;
    }

    return m_signatures[entity];
  }

} // namespace LumiCore::ECS
