#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
namespace LumiCore::Math
{

  /**
   * Calculate a forward direction with euler angles
   *
   * @ return glm::vec3: the forward direction NOT NORMALIZED
   */
  inline glm::vec3 CalculateForwardDirectionEuler(const float yaw, const float pitch)
  {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return front;
  }

  /**
   * Calculate a Right direction
   *
   * @ const glm::vec3 forwardDirection: the forward direction of the object
   * @ const glm::vec3 UpDirection: the up direction of the object
   *
   * @ return glm::vec3: the right direction NOT NORMALIZED
   */
  inline glm::vec3 CalculateRightDirection(const glm::vec3 forwardDirection, const glm::vec3 upDirection)
  {
    return glm::cross(forwardDirection, upDirection);
  }
} // namespace LumiCore::Math
