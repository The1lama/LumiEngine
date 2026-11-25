#pragma once


#include "Entity.h"
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
    size_t m_size;

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
     * Will loopthrough all components that are avalible and remove the entity from them
     */
    void EntityDestoryed(Entity entity);
  };

} // namespace LumiCore::ECS
