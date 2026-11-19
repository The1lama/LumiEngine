#include "Mesh.h"

#include "Shader.h"
#include "logger/Logger.h"
#include <cstddef>
#include <format>
#include <string>
#include <utility>

namespace LumiCore
{

  Mesh::Mesh(const LumiCore::MeshData& data, const GLenum renderType)
  {
    m_indicies = std::move(data.indices);
    m_textures = std::move(data.textures);
    m_renderType = renderType;

    CreateMesh(data);
  }

  Mesh::~Mesh()
  {
    Release();
    // LOG_WARNING("Mesh has been destroyed");
  }

  // Move constructor
  // the noexcept tells the compiler that this funcion is not going to throw exceptions
  Mesh::Mesh(Mesh&& other) noexcept
  {
    m_VAO = std::exchange(other.m_VAO, 0);
    m_VBO = std::exchange(other.m_VBO, 0);
    m_EBO = std::exchange(other.m_EBO, 0);

    m_indicies = std::move(other.m_indicies);
    m_textures = std::move(other.m_textures);

    m_renderType = other.m_renderType;
  }
  Mesh& Mesh::operator=(Mesh&& other) noexcept
  {
    if (this != &other)
    {
      m_VAO = std::exchange(other.m_VAO, 0);
      m_VBO = std::exchange(other.m_VBO, 0);
      m_EBO = std::exchange(other.m_EBO, 0);

      m_indicies = std::move(other.m_indicies);
      m_textures = std::move(other.m_textures);

      m_renderType = other.m_renderType;
    }
    return *this;
  }

  void Mesh::Release()
  {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
  }

  void Mesh::CreateMesh(const LumiCore::MeshData& data)
  {
    constexpr size_t vertexSize = sizeof(Vertex);

    // Create VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertecies.size() * vertexSize, data.vertecies.data(), GL_STATIC_DRAW);

    // Create EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(),
                 GL_STATIC_DRAW);

    constexpr GLsizei stride = sizeof(Vertex);

    // Position Attribute //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // Position Attribute //

    // Normal Attribute //
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    // Normal Attribute //

    // UV Attribute //
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(Vertex, uvCoord)));
    glEnableVertexAttribArray(2);
    // UV Attribute //

    glBindVertexArray(0);
  }

  bool Mesh::IsValid() const
  {
    return glIsVertexArray(m_VAO);
  }


  void Mesh::Render(const Shader& shader)
  {
    if (!IsFunctional())
    {
      LOG_CRITICAL(std::format("Could not load in mesh: {}", m_VAO));
      return;
    }

    // Set the textures to a uniform slot
    for (unsigned int i = 0; i < m_textures.size(); ++i)
    {
      // We can to GL_TEXUTRE0 + 1 becuase of how the macro is layed out in when going
      // inside, i.e. hypotheticla we can increment the texture slot up to 31
      glActiveTexture(GL_TEXTURE0 + i);

      shader.SetInt(("u_Texture_" + std::to_string(i)), i);
      glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0); // Reset the active texture

    // Draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(m_renderType, static_cast<GLsizei>(m_indicies.size()), GL_UNSIGNED_INT, nullptr);

    // unbind the VAO
    glBindVertexArray(0);
  }

} // namespace LumiCore
