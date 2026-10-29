#include "Mesh.h"

#include "Shader.h"
#include "iostream"
namespace LumiCore {

Mesh::Mesh(std::vector<float> vertecies, std::vector<unsigned int> indicies,
           GLenum renderType) {
  m_verticies = vertecies;
  m_verticies.shrink_to_fit();

  m_indicies = indicies;
  m_indicies.shrink_to_fit();

  m_renderType = renderType;

  std::cout << "Verticies size is: " << m_verticies.size()
            << " and indicies size is: " << m_indicies.size() << '\n';

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

void Mesh::CreateMesh(std::vector<float> &vertecies,
                      std::vector<unsigned int> &indicies) {

  // Create VAO
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  // Create VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * sizeof(float),
               vertecies.data(), GL_STATIC_DRAW);

  // Create EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int),
               indicies.data(), GL_STATIC_DRAW);

  constexpr GLsizei stride = 6 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

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
