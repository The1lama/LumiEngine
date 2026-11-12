#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"

namespace LumiCore {

Camera::Camera(const glm::vec3 position, const float fov,
               const float aspectRatio, const glm::vec2 farAndNearPlane) {

  GenerateViewMatrix(position, glm::vec3(0.0f, 0.0f, 0.0f));
  GeneratePerspectiveMatrix(fov, aspectRatio, farAndNearPlane);
}
void Camera::SwitchProjectionMatrix() {
  currentProjectionMatrix = ++currentProjectionMatrix % 2;
}
void Camera::GenerateProjectionMatrix(const float aspectRatio) {}
glm::mat4 Camera::GetViewMatrix() const { return {m_viewMatrix}; }
glm::mat4 Camera::GetProjectionMatrix() const {
  return {currentProjectionMatrix == 0 ? m_perspectiveMatrix
                                       : m_orthographicMatrix};
}

void Camera::GenerateViewMatrix(const glm::vec3 newPosition,
                                const glm::vec3 targetLocation) {

  glm::vec3 cameraDirection = glm::normalize(newPosition - targetLocation);

  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  m_viewMatrix = glm::lookAt(newPosition, targetLocation, worldUp);
}

void Camera::GeneratePerspectiveMatrix(const float fov, const float aspectRatio,
                                       const glm::vec2 farAndNearPlane) {
  // Perspective Matrix //
  m_perspectiveMatrix =
      glm::perspective(fov, aspectRatio, farAndNearPlane.x, farAndNearPlane.y);
}
void Camera::GenerateOrhograpthicMatrix(const float fov,
                                        const float targetDistance,
                                        const float aspectRatio,
                                        const glm::vec2 farAndNearPlane) {
  // Orthographic Matrix //
  constexpr float targDis = 8.0f;

  const float orthHeight = 2.0f * targDis * glm::tan(fov * 0.5f);
  const float orthWidth = orthHeight * aspectRatio;

  const float halfW = orthWidth * 0.5f;
  const float halfH = orthHeight * 0.5f;

  m_orthographicMatrix = glm::ortho(-halfW, halfW, -halfH, halfW,
                                    farAndNearPlane.x, farAndNearPlane.y);
}

} // namespace LumiCore
