#include "Mesh.h"

#include "Shader.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "logger/Logger.h"
#include <cstddef>
#include <format>
#include <utility>

namespace LumiCore {

Mesh::Mesh(LumiCore::MeshData data, GLenum renderType) {

  m_verticies = data.vertecies;
  m_verticies.shrink_to_fit();

  m_indicies = data.indices;
  m_indicies.shrink_to_fit();

  m_renderType = renderType;

  m_location = glm::vec3(0, 0, 0);
  m_rotation = glm::vec3(0, 0, 0);
  m_scale = glm::vec3(1, 1, 1);

  m_transformMatrix = glm::mat4(1.0f);

  CreateMesh(m_verticies, m_indicies);
}

Mesh::~Mesh() {
  Release();
  LOG_WARNING("Mesh has been destroyed");
}

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

  // Position Attribute //
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);
  // Position Attribute //

  // Normal Attribute //
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void *>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(1);
  // Normal Attribute //

  // UV Attribute //
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void *>(offsetof(Vertex, uvCoord)));
  glEnableVertexAttribArray(2);
  // UV Attribute //

  glBindVertexArray(0);
}

bool Mesh::IsValid() const { return glIsVertexArray(m_VAO); }

void Mesh::SetLocation(const glm::vec3 newLocation) {
  m_location = newLocation;
  GenerateTransformationMatrix();
}
void Mesh::SetRotation(const glm::vec3 newRotation) {
  m_rotation = newRotation;
  GenerateTransformationMatrix();
}
void Mesh::SetScale(const glm::vec3 newScale) {
  m_scale = newScale;
  GenerateTransformationMatrix();
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
    LOG_CRITICAL(std::format("Could not load in mesh: {}", m_VAO));
    return;
  }

  glBindVertexArray(m_VAO);

  glDrawElements(m_renderType, static_cast<GLsizei>(m_indicies.size()),
                 GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

} // namespace LumiCore
