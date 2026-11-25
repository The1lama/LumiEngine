#include "Components.h"

#include "../logger/Logger.h"

#include <cassert>
#include <cstddef>
#include <vcruntime_typeinfo.h>

namespace LumiCore::ECS
{

  template <typename T>
  void ComponentArray<T>::InsertData(Entity entity, T component)
  {
    if (EntityExistInEntityToIndexMap(entity))
    {
      LOG_WARNING("Entity already exist with component");
      return;
    }

    size_t newIndex = m_size;
    m_entityToIndexMap[entity] = newIndex;
    m_indexToEntityMap[newIndex] = entity;
    m_componentArray[newIndex] = component;
    ++m_size;
  }

  template <typename T>
  void ComponentArray<T>::RemoveData(Entity entity)
  {
    if (!EntityExistInEntityToIndexMap(entity))
    {
      LOG_WARNING("Entity does not exist in map");
      return;
    }

    // When we remove an component from a entety, we copy over the last element in the component array so that
    // We still have a thightly packed array of components.
    size_t indexOfRemovedEntity = m_entityToIndexMap[entity];
    size_t indexOfLastElement = m_size - 1;
    m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

    // We then update the map values so that the copied values are correct with the map indexes
    Entity entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
    m_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
    m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

    // we can then remove the entity from the unorderdmap
    m_entityToIndexMap.erase(entity);
    m_indexToEntityMap.erase(indexOfLastElement);

    --m_size;
  }

  template <typename T>
  void ComponentArray<T>::EntityDestroyed(Entity entity) /* override */
  {
    if (EntityExistInEntityToIndexMap(entity))
    {
      RemoveData(entity);
    }
  }

  template <typename T>
  T& ComponentArray<T>::GetData(Entity entity)
  {
    if (!EntityExistInEntityToIndexMap(entity)) return;
    return m_componentArray[m_entityToIndexMap[entity]];
  }

  template <typename T>
  bool ComponentArray<T>::EntityExistInEntityToIndexMap(const Entity entity)
  {
    return m_entityToIndexMap.contains(entity);
  }

} // namespace LumiCore::ECS


namespace LumiCore::ECS
{
  template <typename T>
  inline std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray()
  {
    const char* typeName = typeid(T).name();

    assert(m_componentTypes.contains(typeName) && "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
  }

  template <typename T>
  void ComponentManager::RegisterComponent()
  {
    const char* typeName = typeid(T).name();

    assert(m_componentTypes.contains(typeName) && "Registering component type mroe than once");

    m_componentTypes.insert({typeName, m_nextComponentType});
    m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

    ++m_nextComponentType;
  }

  template <typename T>
  T& ComponentManager::GetComponent(Entity entity)
  {
    return GetComponentArray<T>()->GetData(entity);
  }

  template <typename T>
  void ComponentManager::RemoveComponent(Entity entity)
  {
    GetComponentArray<T>()->RemoveData(entity);
  }

  template <typename T>
  void ComponentManager::AddComponent(Entity entity, T component)
  {
    GetComponentArray<T>()->InsertData(entity, component);
  }

  void ComponentManager::EntityDestoryed(Entity entity)
  {
    for (auto const& pair : m_componentArrays)
    {
      auto const& component = pair.second;
      component->EntityDestroyed(entity);
    }
  }

} // namespace LumiCore::ECS
