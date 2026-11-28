#pragma once

#include "../System.h"

class Shader;
namespace LumiCore
{

  namespace Resources
  {
    struct AssetManagers;
  } // namespace Resources

} // namespace LumiCore

namespace LumiCore::ECS::Systems
{

  class RenderSystem : public BaseSystem
  {
  public:
    void Render(Shader& shader, Resources::AssetManagers& resources);
  };

} // namespace LumiCore::ECS::Systems
