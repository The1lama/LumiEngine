#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

namespace LumiCore {

class Camera {

protected:
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_perspectiveMatrix{1.0f};
  glm::mat4 m_orthographicMatrix{1.0f};
  int currentProjectionMatrix{0};

  glm::vec3 m_position{0.0f,0.0f,0.0f};
  glm::vec3 m_targetLocation{0.0f,0.0f,0.0f};

  float m_fov {90};
  float m_aspectRatio{1.77}; // standard 16/9 
  float m_nearPlane{0.01f};
  float m_farPlane{100.0f};

  void GenerateViewMatrix();
  void GenerateProjectionMatixes();
  void GeneratePerspectiveMatrix();
  void GenerateOrhograpthicMatrix();

public:
  Camera(const glm::vec3 position, const float fov, const float aspectRatio,
         const float nearPlane, const float farPlane);
  ~Camera() = default;

  void SwitchProjectionMatrix();

  Camera &SetFov(const float fov);
  Camera &SetAspectRatio(const float aspectRatio);
  Camera &SetLocation(const glm::vec3 newLocation);
  Camera &SetTargetLookAtLocation(const glm::vec3 targetLocation);

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
};

} // namespace LumiCore
