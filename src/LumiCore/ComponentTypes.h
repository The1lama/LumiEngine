#pragma once

#include "Utility.h"
#include "glm/ext/vector_float3.hpp"
#include <string>

namespace LumiCore::Resources
{
  class AssetManagers;
} // namespace LumiCore::Resources

namespace LumiCore
{

  struct Transform
  {
    glm::vec3 Position{0};
    glm::vec3 Rotation{0};
    glm::vec3 Scale{1};
  };

  struct ModelComponent
  {
    std::string Path;
  };

  struct NameComponent
  {
    std::string Name;
  };

  struct TextureComponent
  {
    std::string Path;
  };

  struct Texture
  {
    unsigned int Id;
    std::string Type = "diffuse";
    std::string Path;

    Texture() = default;
    Texture(const std::string& filePath, Resources::AssetManagers& /*unused*/)
    {
      Path = filePath;
      if (!filePath.empty()) Id = Utility::GenerateTexture(filePath);
    }
  };

} // namespace LumiCore


