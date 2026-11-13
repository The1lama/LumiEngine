#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
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
  Vertex(glm::vec3 VertexPosition, glm::vec3 VertexNormal, glm::vec2 UvCoord) :
      position(VertexPosition), normal(VertexNormal), uvCoord(UvCoord) {};
  Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ, float uX, float uY) :
      position(glm::vec3(pX, pY, pZ)), normal(glm::vec3(nX, nY, nZ)), uvCoord(glm::vec2(uX, uY)) {};
};

struct Texture
{
  unsigned int id;
  std::string type;
  std::string path;
};

struct MeshData
{
  std::vector<Vertex> vertecies;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
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
  Mesh(Mesh&& mesh) noexcept;
  Mesh& operator=(Mesh&& mesh) noexcept;

  bool IsValid() const;

  virtual void Render(const Shader& shader);
  virtual void Release();

protected:
  Mesh(LumiCore::MeshData data, GLenum renderType);

  std::vector<Vertex> m_verticies;
  std::vector<unsigned int> m_indicies;
  std::vector<Texture> m_textures;

  GLenum m_renderType;

private:
  GLuint m_VBO, m_VAO, m_EBO;

  void CreateMesh();

  void GenerateTransformationMatrix();

  inline bool IsFunctional() { return m_indicies.size() > 0 && m_VBO != 0; }
};

} // namespace LumiCore
