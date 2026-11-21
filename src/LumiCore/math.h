#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
namespace LumiCore::Math
{

  inline glm::vec3 CalculateForwardDirectionEuler(const float yaw, const float pitch)
  {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return front;
  }
  inline glm::vec3 CalculateRightDirection(const glm::vec3 forwardDirection, const glm::vec3 upDirection)
  {
    return glm::cross(forwardDirection, upDirection);
  }
} // namespace LumiCore::Math
