#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"
#include "logger/Logger.h"

namespace LumiCore
{

Camera::Camera(const glm::vec3 Position, const float FieldOfView, const float AspectRatio, const float NearPlane,
               const float FarPlane) :
    m_position(Position), m_fov(FieldOfView), m_aspectRatio(AspectRatio), m_nearPlane(NearPlane), m_farPlane(FarPlane)
{

  LOG_INFO("Generating View and Projection Matrices");
  GenerateViewMatrix();
  GenerateProjectionMatixes();
}
void Camera::SwitchProjectionMatrix()
{
  m_currentProjectionMatrix = ++m_currentProjectionMatrix % 2;
}

#pragma region Setters
Camera& Camera::SetFov(const float Fov)
{
  m_fov = Fov;
  GenerateProjectionMatixes();
  return *this;
}
Camera& Camera::SetAspectRatio(const float AspectRatio)
{
  m_aspectRatio = AspectRatio;
  GenerateProjectionMatixes();
  return *this;
}
Camera& Camera::SetLocation(const glm::vec3 NewLocation)
{
  m_position = NewLocation;
  GenerateViewMatrix();
  return *this;
}
Camera& Camera::SetTargetLookAtLocation(const glm::vec3 TargetLocation)
{
  m_targetLocation = TargetLocation;
  GenerateViewMatrix();
  return *this;
}
#pragma endregion

#pragma region Getters
glm::mat4 Camera::GetViewMatrix() const
{
  return {m_viewMatrix};
}
glm::mat4 Camera::GetProjectionMatrix() const
{
  return {m_currentProjectionMatrix == 0 ? m_perspectiveMatrix : m_orthographicMatrix};
}
glm::vec3 Camera::GetLocation() const
{
  return m_position;
};
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
  constexpr float TargDis = 8.0f;

  const float OrthHeight = 2.0f * TargDis * glm::tan(m_fov * 0.5f);
  const float OrthWidth = OrthHeight * m_aspectRatio;

  const float HalfW = OrthWidth * 0.5f;
  const float HalfH = OrthHeight * 0.5f;

  m_orthographicMatrix = glm::ortho(-HalfW, HalfW, -HalfH, HalfW, m_nearPlane, m_farPlane);
}
#pragma endregion

} // namespace LumiCore
