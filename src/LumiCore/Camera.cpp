#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"
#include "logger/Logger.h"

namespace LumiCore
{

Camera::Camera(const glm::vec3 position, const float fov, const float aspectRatio, const float nearPlane,
               const float farPlane) :
    m_position(position), m_fov(fov), m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane)
{

  LOG_INFO("Generating View and Projection Matrices");
  GenerateViewMatrix();
  GenerateProjectionMatixes();
}
void Camera::SwitchProjectionMatrix() { currentProjectionMatrix = ++currentProjectionMatrix % 2; }

#pragma region Setters
Camera& Camera::SetFov(const float fov)
{
  m_fov = fov;
  GenerateProjectionMatixes();
  return *this;
}
Camera& Camera::SetAspectRatio(const float aspectRatio)
{
  m_aspectRatio = aspectRatio;
  GenerateProjectionMatixes();
  return *this;
}
Camera& Camera::SetLocation(const glm::vec3 newLocation)
{
  m_position = newLocation;
  GenerateViewMatrix();
  return *this;
}
Camera& Camera::SetTargetLookAtLocation(const glm::vec3 targetLocation)
{
  m_targetLocation = targetLocation;
  GenerateViewMatrix();
  return *this;
}
#pragma endregion

#pragma region Getters
glm::mat4 Camera::GetViewMatrix() const { return {m_viewMatrix}; }
glm::mat4 Camera::GetProjectionMatrix() const
{
  return {currentProjectionMatrix == 0 ? m_perspectiveMatrix : m_orthographicMatrix};
}
#pragma endregion

#pragma region Generate_Matrixes
void Camera::GenerateProjectionMatixes()
{
  GeneratePerspectiveMatrix();
  GenerateOrhograpthicMatrix();
}

void Camera::GenerateViewMatrix()
{
  glm::vec3 cameraDirection = glm::normalize(m_position - m_targetLocation);

  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  m_viewMatrix = glm::lookAt(m_position, m_targetLocation, worldUp);
}

void Camera::GeneratePerspectiveMatrix()
{
  // Perspective Matrix //
  m_perspectiveMatrix = glm::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}
void Camera::GenerateOrhograpthicMatrix()
{
  // Orthographic Matrix //
  constexpr float targDis = 8.0f;

  const float orthHeight = 2.0f * targDis * glm::tan(m_fov * 0.5f);
  const float orthWidth = orthHeight * m_aspectRatio;

  const float halfW = orthWidth * 0.5f;
  const float halfH = orthHeight * 0.5f;

  m_orthographicMatrix = glm::ortho(-halfW, halfW, -halfH, halfW, m_nearPlane, m_farPlane);
}
#pragma endregion

} // namespace LumiCore
