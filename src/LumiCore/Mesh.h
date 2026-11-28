#pragma once

#include "ComponentTypes.h"
#include "ResourceManager.h"
#include "Utility.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

namespace LumiCore
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uvCoord;

    Vertex() = default;
    Vertex(glm::vec3 vertexPosition, glm::vec3 vertexNormal, glm::vec2 uvCoord) :
        position(vertexPosition), normal(vertexNormal), uvCoord(uvCoord) {};
    Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ, float uX, float uY) :
        position(glm::vec3(pX, pY, pZ)), normal(glm::vec3(nX, nY, nZ)), uvCoord(glm::vec2(uX, uY)) {};
  };


  struct MeshData
  {
    std::vector<Vertex> Vertecies;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;
  };

  class Mesh
  {
  public:
    Mesh() = default;
    ~Mesh();

    // Prevenets copying the class
    Mesh(const Mesh& mesh) = delete;
    Mesh& operator=(const Mesh& mesh) = delete;

    // but allows moing
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    bool IsValid() const;

    virtual void Render(const Shader& shader);
    virtual void Release();

  protected:
    Mesh(const LumiCore::MeshData& data, GLenum renderType);


    std::vector<unsigned int> m_indicies;
    std::vector<Texture> m_textures;

    GLenum m_renderType;

  private:
    GLuint m_vbo, m_vao, m_ebo;

    void CreateMesh(const LumiCore::MeshData& data);

    void GenerateTransformationMatrix();

    bool IsFunctional()
    {
      return !m_indicies.empty() && m_vbo != 0;
    }
  };

} // namespace LumiCore
