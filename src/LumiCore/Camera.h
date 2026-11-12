#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

namespace LumiCore {

class Camera {

protected:
  glm::mat4 m_viewMatrix;
  glm::mat4 m_perspectiveMatrix;
  glm::mat4 m_orthographicMatrix;

  int currentProjectionMatrix;

  float m_fov;
  glm::vec3 m_position;
  glm::vec3 m_targetLocation;
  glm::vec3 m_direction;

  void GenerateViewMatrix(const glm::vec3 newPosition,
                          const glm::vec3 targetLocation);

  void GeneratePerspectiveMatrix(const float fov, const float aspectRatio,
                                 const glm::vec2 farAndNearPlane);

  void GenerateOrhograpthicMatrix(const float fov, const float targetDistance,
                                  const float aspectRatio,
                                  const glm::vec2 farAndNearPlane);

public:
  Camera(const glm::vec3 position, const float fov, const float aspectRatio,
         const glm::vec2 farAndNearPlane);
  ~Camera() = default;

  void SwitchProjectionMatrix();

  void GenerateProjectionMatrix(const float aspectRatio);

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
};

} // namespace LumiCore
