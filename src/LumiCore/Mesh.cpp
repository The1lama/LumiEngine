#include "Mesh.h"

#include "ComponentTypes.h"
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
    m_indicies = std::move(data.Indices);
    m_textures = std::move(data.Textures);
    m_renderType = renderType;

    CreateMesh(data);
  }

  Mesh::~Mesh()
  {
    Release();
  }

  // Move constructor
  // the noexcept tells the compiler that this funcion is not going to throw exceptions
  Mesh::Mesh(Mesh&& other) noexcept
  {
    m_vao = std::exchange(other.m_vao, 0);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_ebo = std::exchange(other.m_ebo, 0);

    m_indicies = std::move(other.m_indicies);
    m_textures = std::move(other.m_textures);

    m_renderType = other.m_renderType;
  }
  Mesh& Mesh::operator=(Mesh&& other) noexcept
  {
    if (this != &other)
    {
      m_vao = std::exchange(other.m_vao, 0);
      m_vbo = std::exchange(other.m_vbo, 0);
      m_ebo = std::exchange(other.m_ebo, 0);

      m_indicies = std::move(other.m_indicies);
      m_textures = std::move(other.m_textures);

      m_renderType = other.m_renderType;
    }
    return *this;
  }

  void Mesh::Release()
  {
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
  }

  void Mesh::CreateMesh(const LumiCore::MeshData& data)
  {
    constexpr size_t vertexSize = sizeof(Vertex);

    // Create VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, data.Vertecies.size() * vertexSize, data.Vertecies.data(), GL_STATIC_DRAW);

    // Create EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.Indices.size() * sizeof(unsigned int), data.Indices.data(),
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
    return glIsVertexArray(m_vao);
  }


  void Mesh::Render(const Shader& shader)
  {
    if (!IsFunctional())
    {
      LOG_CRITICAL("Could not load in mesh: {}", m_vao);
      return;
    }

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    // Set the textures to a uniform slot
    const unsigned int size = m_textures.size();

    if (size > 0)
    {
      for (unsigned int i = 0; i < m_textures.size(); ++i)
      {
        // We can to GL_TEXUTRE0 + 1 becuase of how the macro is layed out in when going
        // inside, i.e. hypotheticla we can increment the texture slot up to 31
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i].Type;
        if (name == "diffuse") number = std::to_string(diffuseNr++);
        else if (name == "specular") number = std::to_string(specularNr++);
        else if (name == "normal") number = std::to_string(normalNr++);

        shader.SetInt(("u_Texture_" + name + number), (int)i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].Id);
      }
    }
    glActiveTexture(GL_TEXTURE0); // Reset the active texture

    // Draw mesh
    glBindVertexArray(m_vao);
    glDrawElements(m_renderType, static_cast<GLsizei>(m_indicies.size()), GL_UNSIGNED_INT, nullptr);

    // unbind the VAO
    glBindVertexArray(0);
  }

} // namespace LumiCore
