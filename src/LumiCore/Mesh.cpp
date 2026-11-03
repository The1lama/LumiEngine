#include "Mesh.h"

#include "Shader.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "iostream"
#include <cstddef>
#include <utility>

namespace LumiCore {

Mesh::Mesh(std::vector<Vertex> vertecies, std::vector<unsigned int> indicies,
           GLenum renderType) {
  m_verticies = vertecies;
  m_verticies.shrink_to_fit();

  m_indicies = indicies;
  m_indicies.shrink_to_fit();

  m_renderType = renderType;

  m_location = glm::vec3(0, 0, 0);
  m_rotation = glm::vec3(0, 0, 0);
  m_scale = glm::vec3(1, 1, 1);

  m_transformMatrix = glm::mat4(1.0f);

  CreateMesh(m_verticies, m_indicies);
}

Mesh::~Mesh() { Release(); }

void Mesh::Release() {
  if (m_VBO)
    glDeleteBuffers(1, &m_VBO);
  if (m_VAO)
    glDeleteVertexArrays(1, &m_VAO);
  if (m_EBO)
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::CreateMesh(std::vector<Vertex> &vertecies,
                      std::vector<unsigned int> &indicies) {

  constexpr size_t vertexSize = sizeof(Vertex);

  // Create VAO
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  // Create VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * vertexSize,
               vertecies.data(), GL_STATIC_DRAW);

  // Create EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int),
               indicies.data(), GL_STATIC_DRAW);

  constexpr GLsizei stride = sizeof(Vertex);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
  //                       reinterpret_cast<void *>(offsetof(Vertex,
  //                       position)));
  // glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

Mesh Mesh::SetLocation(const glm::vec3 newLocation) {
  m_location = newLocation;
  GenerateTransformationMatrix();
  return *this;
}
Mesh Mesh::SetRotation(const glm::vec3 newRotation) {
  m_rotation = newRotation;
  GenerateTransformationMatrix();
  return *this;
}
Mesh Mesh::SetScale(const glm::vec3 newScale) {
  m_scale = newScale;
  GenerateTransformationMatrix();
  return *this;
}

void Mesh::GenerateTransformationMatrix() {
  auto upVector = glm::vec3(0, 0, 1);
  glm::mat4 newTrans = glm::mat4(1.0f);
  newTrans = glm::translate(newTrans, m_location);
  // newTrans =
  //     glm::rotate(newTrans, CalculateAngleFromVector3(m_rotation), upVector);
  newTrans = glm::scale(newTrans, m_scale);
  m_transformMatrix = std::move(newTrans);
}

float CalculateAngleFromVector3(const glm::vec3 vectorAngles) { return 0; }

void Mesh::Render(Shader &shader) {
  if (!IsFunctional()) {
    std::cout << "Could not load a mesh" << '\n';
    return;
  }

  glBindVertexArray(m_VAO);

  shader.SetFloat("uTime", m_time);
  shader.SetFloat("xPosOffset", m_xyOffsets.x);
  shader.SetFloat("yPosOffset", m_xyOffsets.y);
  shader.SetFloat("xScale", m_xyScale.x);
  shader.SetFloat("yScale", m_xyScale.y);
  shader.SetVec3("uColor", m_color);
  shader.SetFloat("uRotationAngle", m_angle);

  glDrawElements(m_renderType, static_cast<GLsizei>(m_indicies.size()),
                 GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

} // namespace LumiCore
