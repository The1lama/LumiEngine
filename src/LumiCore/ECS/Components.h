#pragma once

#include "../ComponentTypes.h"
#include "ECSTypes.h"

#include <array>
#include <cstddef>
#include <memory>
#include <unordered_map>

namespace LumiCore::ECS
{

  class IComponentArray
  {
  public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
  };

  template <typename T>
  class ComponentArray : public IComponentArray
  {
  private:
    // This is a array of the Component type (Transform, Gravity, ...)
    // that holds all entities that are using this component in
    std::array<T, MaxEntities> m_componentArray;

    // the unordered maps can be changed out to arrays as they are giving a performace penalty as the map
    // elements are not in order.
    std::unordered_map<Entity, size_t> m_entityToIndexMap;
    std::unordered_map<size_t, Entity> m_indexToEntityMap;

    // The amount of entities that are in the component array
    size_t m_size{0};

    // Helper function to check if an entity is using his component
    bool EntityExistInEntityToIndexMap(const Entity entity);

  public:
    /**
     * Adds the entity to the component
     */
    void InsertData(Entity entity, T component);

    /**
     * Removes the entity from the component.
     * It will move the last avalible entity to the last position to the newly
     * freed position, so that the component still has a tightly packed array
     */
    void RemoveData(Entity entity);

    /**
     * basically an overload function for RemoveData()
     */
    void EntityDestroyed(Entity entity) override;

    /**
     * returns the data that the entity is owning.
     */
    T& GetData(Entity entity);
  };

  template <typename T>
  inline void ComponentArray<T>::InsertData(Entity entity, T component)
  {
    assert(!EntityExistInEntityToIndexMap(entity));
    assert(m_size < MaxEntities);

    size_t newIndex = m_size;
    m_entityToIndexMap[entity] = newIndex;
    m_indexToEntityMap[newIndex] = entity;
    m_componentArray[newIndex] = component;
    ++m_size;
  }

  template <typename T>
  inline void ComponentArray<T>::RemoveData(Entity entity)
  {
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
  inline void ComponentArray<T>::EntityDestroyed(Entity entity) /* override */
  {
    if (EntityExistInEntityToIndexMap(entity))
    {
      RemoveData(entity);
    }
  }

  template <typename T>
  inline T& ComponentArray<T>::GetData(Entity entity)
  {
    assert(EntityExistInEntityToIndexMap(entity));
    return m_componentArray[m_entityToIndexMap.at(entity)];
  }

  template <typename T>
  inline bool ComponentArray<T>::EntityExistInEntityToIndexMap(const Entity entity)
  {
    return m_entityToIndexMap.contains(entity);
  }

  ////////////////////////

  class ComponentManager
  {
  private:
    std::unordered_map<const char*, ComponentType> m_componentTypes;
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_componentArrays;

    // this starts at 0
    ComponentType m_nextComponentType;

    // Helper function for getting the component array
    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray();

  public:
    /**
     * Register new component, this is a must if we are creating a new compoent that we want to use.
     * This will add the component type to an unordered_map where the type name is the key.
     */
    template <typename T>
    void RegisterComponent();

    /**
     * Adds a component to the entity
     */
    template <typename T>
    void AddComponent(Entity entity, T component);

    /**
     * Removes the component from the entity, usefull if we want to remove reset a
     * entity with other components
     */
    template <typename T>
    void RemoveComponent(Entity entity);

    /**
     * Get the requested component from the entity
     */
    template <typename T>
    T& GetComponent(Entity entity);

    /**
     * return the components type
     */
    template <typename T>
    ComponentType GetComponentType();

    /**
     * Will loopthrough all components that are avalible and remove the entity from them
     */
    void EntityDestoryed(Entity entity);
  };

  template <typename T>
  inline std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray()
  {
    const char* typeName = typeid(T).name();
    auto it = m_componentArrays.find(typeid(T).name());
    assert(it != m_componentArrays.end() && "Component type not registered.");
    assert(it->second != nullptr && "ComponentArray pointer is null.");
    return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
  }

  template <typename T>
  inline void ComponentManager::RegisterComponent()
  {
    const char* typeName = typeid(T).name();

    m_componentTypes.insert({typeName, m_nextComponentType});
    m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

    ++m_nextComponentType;
  }

  template <typename T>
  inline T& ComponentManager::GetComponent(Entity entity)
  {
    return GetComponentArray<T>()->GetData(entity);
  }

  template <typename T>
  inline ComponentType ComponentManager::GetComponentType()
  {
    const char* typeName = typeid(T).name();

    return m_componentTypes[typeName];
  }


  template <typename T>
  inline void ComponentManager::RemoveComponent(Entity entity)
  {
    GetComponentArray<T>()->RemoveData(entity);
  }

  template <typename T>
  inline void ComponentManager::AddComponent(Entity entity, T component)
  {
    GetComponentArray<T>()->InsertData(entity, component);
  }
} // namespace LumiCore::ECS
