#pragma once

#include "ECSTypes.h"

#include <array>
#include <cstdint>
#include <stack>
#include <vector>

namespace LumiCore::ECS
{

  class EntityManager
  {
  private:
    std::stack<Entity> m_freeEntities;
    std::array<Signature, MaxEntities> m_signatures;
    std::vector<Entity> m_aliveEntities;
    uint32_t m_livingEntityCount {0};

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

    const std::vector<Entity>& GetAliveEntities() const;
  };

} // namespace LumiCore::ECS
