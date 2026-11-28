#pragma once

// Type alias for entity (Can be changed for other types if needed)
#include <bitset>
#include <cstdint>

namespace LumiCore::ECS
{
  using Entity = std::uint32_t;

  // Set the amount of entities we can use. (This is just an abritrary number that I have set)
  inline extern const Entity MaxEntities = 5000;

  // Type alias for each component (Can be changed for other types if needed)
  using ComponentType = std::uint8_t;

  // Set the max amount of components we can use (This is just an abritrary number that I have set)
  inline constexpr ComponentType maxComponents = 32;

  // What components the entity is using for faster lookup as it is using bits
  // The MaxComponents is the number of bits are are allocating storage for
  using Signature = std::bitset<maxComponents>;

} // namespace LumiCore::ECS
