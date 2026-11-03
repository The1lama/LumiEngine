#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

namespace LumiCore {

struct Vertex {
  glm::vec3 position;

  Vertex(glm::vec3 VertexPosition) : position(VertexPosition) {};
  Vertex(float x, float y, float z) : position(glm::vec3(x, y, z)) {};
};

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<Vertex> vertecies, std::vector<unsigned int> indicies,
       GLenum renderType);
  ~Mesh();

  void SetTime(const float time) { m_time = time; }
  void SetAngle(const float angle) { m_angle = angle; }
  void SetOffset(const glm::vec2 offset) { m_xyOffsets = offset; }
  void SetScale(const glm::vec2 scale) { m_xyScale = scale; }
  void SetColor(const glm::vec3 color) { m_color = color; }

  Mesh SetLocation(const glm::vec3 newLocation);
  Mesh SetRotation(const glm::vec3 newRotation);
  Mesh SetScale(const glm::vec3 newScale);

  virtual void Render(Shader &shader);
  virtual void Release();

protected:
  std::vector<Vertex> m_verticies;
  std::vector<unsigned int> m_indicies;

  GLenum m_renderType;

  glm::vec3 m_location;
  glm::vec3 m_rotation;
  glm::vec3 m_scale;

  glm::mat4 m_transformMatrix;

private:
  float m_time = 1.f;
  float m_angle = .0f;
  glm::vec2 m_xyOffsets = glm::vec2(0, 0);
  glm::vec2 m_xyScale = glm::vec2(1, 1);
  glm::vec3 m_color = glm::vec3(0, 0, 0);

  GLuint m_VBO, m_VAO, m_EBO;

  void CreateMesh(std::vector<Vertex> &vertecies,
                  std::vector<unsigned int> &indicies);

  void GenerateTransformationMatrix();

  inline bool IsFunctional() { return m_indicies.size() > 0 && m_VBO != 0; }
};

} // namespace LumiCore
