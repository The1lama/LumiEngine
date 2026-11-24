#pragma once

#include <glm/vec3.hpp>
#include <string>

namespace Objects
{
  struct Transform
  {
    glm::vec3 Position{0};
    glm::vec3 Rotation{0};
    glm::vec3 Scale{1};
  };

  struct GameObject
  {
    std::string ModelPath;
    std::string TexturePath;
    Objects::Transform Transform;
  };

} // namespace Objects


