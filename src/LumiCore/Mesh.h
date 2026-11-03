#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

namespace LumiCore {

struct Vertex {
  glm::vec3 Position;
  glm::vec3 NormalDirection;
  glm::vec2 TexUV;
};

class Mesh {
public:
  Mesh() = default;
  Mesh(std::vector<float> vertecies, std::vector<unsigned int> indicies,
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

  float CalculateAngleFromVector3(const glm::vec3 vectorAngles);

  virtual void Render(Shader &shader);
  virtual void Release();

protected:
  std::vector<float> m_verticies;
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

  void CreateMesh(std::vector<float> &vertecies,
                  std::vector<unsigned int> &indicies);

  void GenerateTransformationMatrix();

  inline bool IsFunctional() { return m_indicies.size() > 0 && m_VBO != 0; }
};

} // namespace LumiCore
