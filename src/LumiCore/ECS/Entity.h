#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <stack>

namespace LumiCore::ECS
{
  // Type alias for entity (Can be changed for other types if needed)
  using Entity = std::uint32_t;

  // Set the amount of entities we can use. (This is just an abritrary number that I have set)
  inline extern const Entity MaxEntities = 5000;

  // Type alias for each component (Can be changed for other types if needed)
  using ComponentType = std::uint8_t;

  // Set the max amount of components we can use (This is just an abritrary number that I have set)
  inline const ComponentType MaxComponents = 32;

  // What components the entity is using for faster lookup as it is using bits
  // The MaxComponents is the number of bits are are allocating storage for
  using Signature = std::bitset<MaxComponents>;

  class EntityManager
  {
  private:
    std::stack<Entity> m_freeEntities;
    std::array<Signature, MaxEntities> m_signatures;
    uint32_t m_livingEntityCount;

  public:
    EntityManager();

    /**
     * Create a new entity
     *
     * @ return Entity: Returns the entity ID.
     */
    Entity CreateEntity();

    /**
     * We are putting the entity to avalible entites stack so that we can reuse it
     */
    void DestroyEntity(Entity entity);

    /**
     * Set a signature for an entity
     *
     * @ Entity entity: The entity we want to set the signature to
     * @ Signature signature: The signature we want to set on the entity
     */
    void SetSignature(Entity entity, Signature signature);

    /**
     * Get the signature for the entity
     *
     * @ return Signature: The signature that the entity has
     */
    Signature GetSignature(Entity entity);
  };

} // namespace LumiCore::ECS
